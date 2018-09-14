#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <asio/high_resolution_timer.hpp>
#include <asio/io_service.hpp>
#include <beam/message/buffer_pool.hpp>
#include <beam/message/capnproto.hpp>
#include <beam/message/capnproto.hh>
#include <kj/common.h>
#include <kj/debug.h>
#include <kj/main.h>
#include <kj/string.h>
#include <glog/logging.h>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/container/spsc_ring_queue.hh>
#include <turbo/ipc/posix/pipe.hpp>
#include <turbo/ipc/posix/signal_notifier.hpp>
#include <turbo/process/posix/spawn.hpp>
#include <robocup2Dsim/common/metadata.hpp>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <robocup2Dsim/srprotocol/protocol.hpp>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include "client_io.hpp"
#include "config.hpp"
#include "event.hpp"
#include "event.hh"
#include "ref_io.hpp"

namespace bin = beam::internet;
namespace bme = beam::message;
namespace bmc = beam::message::capnproto;
namespace tip = turbo::ipc::posix;
namespace tpp = turbo::process::posix;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;
namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;
namespace rse = robocup2Dsim::server;
namespace rsr = robocup2Dsim::srprotocol;

void parse_cmd_args(int argc, char* argv[], rse::config& conf)
{
    kj::TopLevelProcessContext context(argv[0]);
    kj::MainFunc parse = kj::MainBuilder(
	    context,
	    "Robocup 2D Simulation Server v0.0",
	    "Launches ref and waits for clients to connect")
	.expectArg("address", [&] (kj::StringPtr arg)
	{
	    conf.address = arg.cStr();
	    ++conf.ref_arg_offset;
	    return true;
	})
	.expectArg("port", [&] (kj::StringPtr arg)
	{
	    int tmp = atoi(arg.cStr());
	    if (std::numeric_limits<decltype(conf.port)>::min() <= tmp && 
		    tmp <= std::numeric_limits<decltype(conf.port)>::max())
	    {
		conf.port = static_cast<decltype(conf.port)>(tmp);
		++conf.ref_arg_offset;
		return kj::MainBuilder::Validity(true);
	    }
	    else
	    {
		return kj::MainBuilder::Validity(kj::str(
			"port number must be int the range ",
			std::numeric_limits<decltype(conf.port)>::min(),
			" to ",
			std::numeric_limits<decltype(conf.port)>::max()));
	    }
	})
	.expectArg("ref_path", [&] (kj::StringPtr arg)
	{
	    conf.ref_path = arg.cStr();
	    ++conf.ref_arg_offset;
	    return true;
	})
	.expectZeroOrMoreArgs("ref_arg", [&] (kj::StringPtr)
	{
	    ++conf.ref_arg_count;
	    return true;
	})
	.build();
    KJ_STACK_ARRAY(kj::StringPtr, params, argc - 1, 8, 32);
    for (int iter = 1; iter < argc; ++iter)
    {
	params[iter - 1] = argv[iter];
    }
    parse(argv[0], params);
}

class server
{
public:
    server(const rse::config& config, tpp::child&& ref);
    void run();
private:
    typedef std::chrono::high_resolution_clock clock_type;
    const rse::config config_;
    rsr::ref_input_queue_type ref_input_queue_;
    rsr::ref_output_queue_type ref_output_queue_;
    rcs::server_status_queue_type server_status_queue_;
    rcs::server_trans_queue_type server_trans_queue_;
    rcs::client_status_queue_type client_status_queue_;
    rcs::client_trans_queue_type client_trans_queue_;
    rsr::ref_input_queue_type::producer& ref_input_producer_;
    rsr::ref_output_queue_type::consumer& ref_output_consumer_;
    rcs::server_status_queue_type::producer& server_status_producer_;
    rcs::server_trans_queue_type::producer& server_trans_producer_;
    rcs::client_status_queue_type::consumer& client_status_consumer_;
    rcs::client_trans_queue_type::consumer& client_trans_consumer_;
    rse::event::basic_handle handle_;
    tip::signal_notifier notifier_;
    tpp::child ref_;
    rse::ref_io ref_io_;
    rse::client_io client_io_;
};

server::server(const rse::config& config, tpp::child&& ref) :
	config_(config),
	ref_input_queue_(config_.ref_msg_queue_length),
	ref_output_queue_(config_.ref_msg_queue_length),
	server_status_queue_(config_.client_msg_queue_length),
	server_trans_queue_(config_.client_msg_queue_length),
	client_status_queue_(config_.client_msg_queue_length),
	client_trans_queue_(config_.client_msg_queue_length),
	ref_input_producer_(ref_input_queue_.get_producer()),
	ref_output_consumer_(ref_output_queue_.get_consumer()),
	server_status_producer_(server_status_queue_.get_producer()),
	server_trans_producer_(server_trans_queue_.get_producer()),
	client_status_consumer_(client_status_queue_.get_consumer()),
	client_trans_consumer_(client_trans_queue_.get_consumer()),
	handle_
	{
	    &ref_input_producer_,
	    &ref_output_consumer_,
	    &server_status_producer_,
	    &server_trans_producer_,
	    &client_status_consumer_,
	    &client_trans_consumer_,
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.ref_msg_word_length, config_.ref_msg_buffer_capacity))),
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.ref_msg_word_length, config_.ref_msg_buffer_capacity))),
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.client_msg_word_length, config_.client_msg_buffer_capacity))),
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.client_msg_word_length, config_.client_msg_buffer_capacity))),
	    rse::event::state::noref_waiting
	},
	notifier_(),
	ref_(std::move(ref)),
	ref_io_(config_, ref_.in, ref_.out, ref_input_queue_.get_consumer(), ref_output_queue_.get_producer()),
	client_io_(
		client_status_queue_.get_producer(),
		client_trans_queue_.get_producer(),
		server_status_queue_.get_consumer(),
		server_trans_queue_.get_consumer(),
		config_)
{
    // TODO: setup SIGCHLD handling
    std::unique_ptr<ren::physics> physics(new ren::physics());
    ren::register_system(rru::update_local_db(), std::move(physics));
}

void server::run()
{
    rse::event::with(std::move(handle_),
	[&](rse::event::handle<rse::event::state::noref_waiting>&& handle)
	{
	    handle_ = std::move(rse::event::ref_spawned(std::move(handle), config_));
	}
    );
    bmc::payload<rsr::RefOutput> ref_payload;
    bmc::payload<rcs::ClientStatus> status_payload;
    bmc::payload<rcs::ClientTransaction> trans_payload;
    rco::frame_number frame = 0;
    clock_type::time_point next_frame_time = clock_type::now() + std::chrono::microseconds(config_.frame_duration);
    bool should_run = true;
    while (should_run)
    {
        clock_type::time_point now = clock_type::now();
        if (now < next_frame_time)
        {
            continue;
        }
        ++frame;
	if (ref_output_consumer_.try_dequeue_move(ref_payload) == rsr::ref_output_queue_type::consumer::result::success)
	{
	    bmc::statement<rsr::RefOutput> ref_output(std::move(ref_payload));
	    rse::event::with(std::move(handle_),
		[&](rse::event::handle<rse::event::state::noref_waiting>&& handle)
		{
		    if (ref_output.read().isRefReady())
		    {
			handle_ = std::move(rse::event::ref_spawned(std::move(handle), config_));
		    }
		},
		[&](rse::event::handle<rse::event::state::withref_waiting>&& handle)
		{
		    if (ref_output.read().isFieldOpen())
		    {
			handle_ = std::move(rse::event::field_opened(std::move(handle), ref_output.read().getFieldOpen()));
		    }
		    else if (ref_output.read().isRosterFinalised())
		    {
			handle_ = std::move(rse::event::roster_finalised(std::move(handle)));
		    }
		    else if (ref_output.read().isRefCrashed())
		    {
			handle_ = std::move(rse::event::ref_crashed(std::move(handle)));
		    }
		},
		[&](rse::event::handle<rse::event::state::withref_playing>&& handle)
		{
		    if (ref_output.read().isPlayJudgement())
		    {
			handle_ = std::move(rse::event::play_judged(std::move(handle), ref_output.read().getPlayJudgement()));
		    }
		    else if (ref_output.read().isMatchClose())
		    {
			handle_ = std::move(rse::event::match_closed(std::move(handle), ref_output.read().getMatchClose()));
		    }
		    else if (ref_output.read().isMatchAbort())
		    {
			handle_ = std::move(rse::event::match_aborted(std::move(handle), ref_output.read().getMatchAbort()));
		    }
		    else if (ref_output.read().isRefCrashed())
		    {
			handle_ = std::move(rse::event::ref_crashed(std::move(handle)));
		    }
		},
		[&](rse::event::handle<rse::event::state::noref_playing>&& handle)
		{
		    if (ref_output.read().isRefReady())
		    {
			handle_ = std::move(rse::event::ref_spawned(std::move(handle), config_));
		    }
		}
	    );
	}
	if (client_status_consumer_.try_dequeue_move(status_payload) == rcs::client_status_queue_type::consumer::result::success)
	{
	    bmc::statement<rcs::ClientStatus> client_status(std::move(status_payload));
	    rse::event::with(std::move(handle_),
		[&](rse::event::handle<rse::event::state::withref_playing>&& handle)
		{
		    handle_ = std::move(rse::event::status_uploaded(std::move(handle), client_status.read()));
		},
		[&](rse::event::handle<rse::event::state::noref_playing>&& handle)
		{
		    handle_ = std::move(rse::event::status_uploaded(std::move(handle), client_status.read()));
		}
	    );
	}
	if (client_trans_consumer_.try_dequeue_move(trans_payload) == rcs::client_trans_queue_type::consumer::result::success)
	{
	    bmc::statement<rcs::ClientTransaction> client_trans(std::move(trans_payload));
	    rse::event::with(std::move(handle_),
		[&](rse::event::handle<rse::event::state::noref_waiting>&& handle)
		{
		    if (client_trans.read().isRegistration())
		    {
			handle_ = std::move(rse::event::registration_requested(
				std::move(handle),
				client_trans.get_source(),
				client_trans.read().getRegistration()));
		    }
		    else if (client_trans.read().isDisconnect())
		    {
			// FIXME: disconnected needs more information
			handle_ = std::move(rse::event::disconnected(std::move(handle)));
		    }
		},
		[&](rse::event::handle<rse::event::state::withref_waiting>&& handle)
		{
		    if (client_trans.read().isRegistration())
		    {
			handle_ = std::move(rse::event::registration_requested(
				std::move(handle),
				client_trans.get_source(),
				client_trans.read().getRegistration()));
		    }
		    else if (client_trans.read().isDisconnect())
		    {
			// FIXME: disconnected needs more information
			handle_ = std::move(rse::event::disconnected(std::move(handle)));
		    }
		},
		[&](rse::event::handle<rse::event::state::withref_playing>&& handle)
		{
		    if (client_trans.read().isAction())
		    {
			handle_ = std::move(rse::event::control_actioned(std::move(handle), client_trans.read().getAction()));
		    }
		    else if (client_trans.read().isRegistration())
		    {
			handle_ = std::move(rse::event::registration_requested(
				std::move(handle),
				client_trans.get_source(),
				client_trans.read().getRegistration()));
		    }
		    else if (client_trans.read().isDisconnect())
		    {
			// FIXME: disconnected needs more information
			handle_ = std::move(rse::event::disconnected(std::move(handle)));
		    }
		},
		[&](rse::event::handle<rse::event::state::noref_playing>&& handle)
		{
		    if (client_trans.read().isAction())
		    {
			handle_ = std::move(rse::event::control_actioned(std::move(handle), client_trans.read().getAction()));
		    }
		    else if (client_trans.read().isRegistration())
		    {
			handle_ = std::move(rse::event::registration_requested(
				std::move(handle),
				client_trans.get_source(),
				client_trans.read().getRegistration()));
		    }
		    else if (client_trans.read().isDisconnect())
		    {
			// FIXME: disconnected needs more information
			handle_ = std::move(rse::event::disconnected(std::move(handle)));
		    }
		}
	    );
	}
	rse::event::with(std::move(handle_),
	    [&](rse::event::handle<rse::event::state::withref_onbreak>&& handle)
	    {
		if (frame % config_.ping_frequency == 0U)
		{
		    handle_ = std::move(rse::event::ping_timedout(std::move(handle)));
		}
            },
	    [&](rse::event::handle<rse::event::state::noref_onbreak>&& handle)
	    {
		if (frame % config_.ping_frequency == 0U)
		{
		    handle_ = std::move(rse::event::ping_timedout(std::move(handle)));
		}
            },
	    [&](rse::event::handle<rse::event::state::withref_playing>&& handle)
	    {
		if (frame % config_.simulation_frequency == config_.simulation_start_frame)
		{
		    handle_ = std::move(rse::event::simulation_timedout(std::move(handle)));
		}
		if (frame % config_.snapshot_frequency == config_.snapshot_start_frame)
		{
		    handle_ = std::move(rse::event::snapshot_timedout(std::move(handle)));
		}
	    },
	    [&](rse::event::handle<rse::event::state::noref_playing>&& handle)
	    {
		if (frame % config_.simulation_frequency == config_.simulation_start_frame)
		{
		    handle_ = std::move(rse::event::simulation_timedout(std::move(handle)));
		}
		if (frame % config_.snapshot_frequency == config_.snapshot_start_frame)
		{
		    handle_ = std::move(rse::event::snapshot_timedout(std::move(handle)));
		}
	    }
	);
	next_frame_time += std::chrono::microseconds(config_.frame_duration);
	if (clock_type::now() > next_frame_time)
	{
	    std::time_t time = clock_type::to_time_t(next_frame_time);
	    LOG(WARNING) << "Frame number " << frame << " at " << ctime(&time) << " missed";
	}
    }
}

int main(int argc, char* argv[])
{
    rse::config conf;
    parse_cmd_args(argc, argv, conf);
    tpp::child ref = tpp::spawn(conf.ref_path.c_str(), &argv[conf.ref_arg_offset], {}, 2 << 16);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = (kj::_::Debug::shouldLog(kj::_::Debug::Severity::INFO)) ? 0 : 1;
    google::InstallFailureSignalHandler();
    server serv(std::move(conf), std::move(ref));
    serv.run();
    return 0;
}

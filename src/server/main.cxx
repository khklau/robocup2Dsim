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
#include <beam/message/capnproto.hpp>
#include <beam/message/capnproto.hxx>
#include <kj/common.h>
#include <kj/debug.h>
#include <kj/main.h>
#include <kj/string.h>
#include <glog/logging.h>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/container/spsc_ring_queue.hxx>
#include <turbo/ipc/posix/pipe.hpp>
#include <turbo/ipc/posix/signal_notifier.hpp>
#include <turbo/process/posix/spawn.hpp>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <robocup2Dsim/srprotocol/protocol.hpp>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include "client_io.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "engine.hxx"
#include "ref_io.hpp"

namespace bme = beam::message;
namespace tip = turbo::ipc::posix;
namespace tpp = turbo::process::posix;
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
    const rse::config config_;
    rse::engine::basic_handle handle_;
    tip::signal_notifier notifier_;
    tpp::child ref_;
    rse::ref_io ref_io_;
    rse::client_io client_io_;
};

server::server(const rse::config& config, tpp::child&& ref) :
	config_(config),
	handle_
	{
	    std::move(std::unique_ptr<rsr::ref_input_queue_type>(new rsr::ref_input_queue_type(config_.ref_msg_queue_length))),
	    std::move(std::unique_ptr<rsr::ref_output_queue_type>(new rsr::ref_output_queue_type(config_.ref_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::client_status_queue_type>(new rcs::client_status_queue_type(config_.client_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::client_trans_queue_type>(new rcs::client_trans_queue_type(config_.client_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::server_status_queue_type>(new rcs::server_status_queue_type(config_.client_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::server_trans_queue_type>(new rcs::server_trans_queue_type(config_.client_msg_queue_length))),
	    rse::engine::state::withref_waiting
	},
	notifier_(),
	ref_(std::move(ref)),
	ref_io_(ref_.in, ref_.out, handle_.ref_input_queue->get_consumer(), handle_.ref_output_queue->get_producer()),
	client_io_(
		handle_.client_status_queue->get_producer(),
		handle_.client_trans_queue->get_producer(),
		handle_.server_status_queue->get_consumer(),
		handle_.server_trans_queue->get_consumer(),
		config_)
{
    // TODO: setup SIGCHLD handling
    std::unique_ptr<ren::physics> physics(new ren::physics());
    ren::register_system(rru::update_local_db(), std::move(physics));
}

void server::run()
{
    std::unique_ptr<bme::capnproto<rsr::RefOutput>> ref_output;
    std::unique_ptr<bme::capnproto<rcs::ClientStatus>> client_status;
    std::unique_ptr<bme::capnproto<rcs::ClientTransaction>> client_trans;
    std::chrono:: high_resolution_clock::time_point next_tick = std::chrono::high_resolution_clock::now() + config_.tick_rate;
    asio::io_service service;
    asio::high_resolution_timer timer(service);
    std::size_t tick_count = 0;
    bool should_run = true;
    while (should_run)
    {
	timer.expires_at(next_tick);
	timer.wait();
	if (handle_.ref_output_queue->get_consumer().try_dequeue_move(ref_output) == rsr::ref_output_queue_type::consumer::result::success)
	{
	    if (handle_.engine_state == rse::engine::state::noref_waiting)
	    {
		if (ref_output->get_reader().isRefReady())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::ref_ready(
			    rse::engine::down_cast<rse::engine::state::noref_waiting>(std::move(handle_)))));
		}
	    }
	    if (handle_.engine_state == rse::engine::state::withref_waiting)
	    {
		if (ref_output->get_reader().isFieldOpen())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::field_opened(
			    rse::engine::down_cast<rse::engine::state::withref_waiting>(std::move(handle_)),
			    ref_output->get_reader().getFieldOpen())));
		}
		if (ref_output->get_reader().isRefCrashed())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::ref_crashed(
			    rse::engine::down_cast<rse::engine::state::withref_waiting>(std::move(handle_)))));
		}
	    }
	    if (handle_.engine_state == rse::engine::state::withref_playing)
	    {
		if (ref_output->get_reader().isPlayJudgement())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::play_judged(
			    rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)),
			    ref_output->get_reader().getPlayJudgement())));
		}
		if (ref_output->get_reader().isMatchClose())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::match_closed(
			    rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)),
			    ref_output->get_reader().getMatchClose())));
		}
		if (ref_output->get_reader().isMatchAbort())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::match_aborted(
			    rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)),
			    ref_output->get_reader().getMatchAbort())));
		}
		if (ref_output->get_reader().isRefCrashed())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::ref_crashed(
			    rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)))));
		}
	    }
	    if (handle_.engine_state == rse::engine::state::noref_playing)
	    {
		if (ref_output->get_reader().isRefReady())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::ref_ready(
			    rse::engine::down_cast<rse::engine::state::noref_playing>(std::move(handle_)))));
		}
	    }
	}
	if (handle_.client_status_queue->get_consumer().try_dequeue_move(client_status) == rcs::client_status_queue_type::consumer::result::success)
	{
	    if (handle_.engine_state == rse::engine::state::withref_playing)
	    {
		handle_ = std::move(rse::engine::up_cast(rse::engine::status_uploaded(
			rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)),
			client_status->get_reader())));
	    }
	    if (handle_.engine_state == rse::engine::state::noref_playing)
	    {
		handle_ = std::move(rse::engine::up_cast(rse::engine::status_uploaded(
			rse::engine::down_cast<rse::engine::state::noref_playing>(std::move(handle_)),
			client_status->get_reader())));
	    }
	}
	if (handle_.client_trans_queue->get_consumer().try_dequeue_move(client_trans) == rcs::client_trans_queue_type::consumer::result::success)
	{
	    if (handle_.engine_state == rse::engine::state::noref_waiting)
	    {
		if (client_trans->get_reader().isRegistration())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::registration_requested(
			    rse::engine::down_cast<rse::engine::state::noref_waiting>(std::move(handle_)),
			    client_trans->get_reader().getRegistration())));
		}
		if (client_trans->get_reader().isDisconnect())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::disconnected(
			    rse::engine::down_cast<rse::engine::state::noref_waiting>(std::move(handle_)))));
		}
	    }
	    if (handle_.engine_state == rse::engine::state::withref_waiting)
	    {
		if (client_trans->get_reader().isRegistration())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::registration_requested(
			    rse::engine::down_cast<rse::engine::state::withref_waiting>(std::move(handle_)),
			    client_trans->get_reader().getRegistration())));
		}
		if (client_trans->get_reader().isDisconnect())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::disconnected(
			    rse::engine::down_cast<rse::engine::state::withref_waiting>(std::move(handle_)))));
		}
	    }
	    if (handle_.engine_state == rse::engine::state::withref_playing)
	    {
		if (client_trans->get_reader().isAction())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::control_actioned(
			    rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)),
			    client_trans->get_reader().getAction())));
		}
		if (client_trans->get_reader().isRegistration())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::registration_requested(
			    rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)),
			    client_trans->get_reader().getRegistration())));
		}
		if (client_trans->get_reader().isDisconnect())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::disconnected(
			    rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)))));
		}
	    }
	    if (handle_.engine_state == rse::engine::state::noref_playing)
	    {
		if (client_trans->get_reader().isAction())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::control_actioned(
			    rse::engine::down_cast<rse::engine::state::noref_playing>(std::move(handle_)),
			    client_trans->get_reader().getAction())));
		}
		if (client_trans->get_reader().isRegistration())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::registration_requested(
			    rse::engine::down_cast<rse::engine::state::noref_playing>(std::move(handle_)),
			    client_trans->get_reader().getRegistration())));
		}
		if (client_trans->get_reader().isDisconnect())
		{
		    handle_ = std::move(rse::engine::up_cast(rse::engine::disconnected(
			    rse::engine::down_cast<rse::engine::state::noref_playing>(std::move(handle_)))));
		}
	    }
	}
	if (handle_.engine_state == rse::engine::state::withref_playing)
	{
	    if (tick_count % config_.simulation_frequency == config_.simulation_start_tick)
	    {
		handle_ = std::move(rse::engine::up_cast(rse::engine::simulation_timedout(
			rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)))));
	    }
	    if (tick_count % config_.snapshot_frequency == config_.snapshot_start_tick)
	    {
		handle_ = std::move(rse::engine::up_cast(rse::engine::snapshot_timedout(
			rse::engine::down_cast<rse::engine::state::withref_playing>(std::move(handle_)))));
	    }
	}
	if (handle_.engine_state == rse::engine::state::noref_playing)
	{
	    if (tick_count % config_.simulation_frequency == config_.simulation_start_tick)
	    {
		handle_ = std::move(rse::engine::up_cast(rse::engine::simulation_timedout(
			rse::engine::down_cast<rse::engine::state::noref_playing>(std::move(handle_)))));
	    }
	    if (tick_count % config_.snapshot_frequency == config_.snapshot_start_tick)
	    {
		handle_ = std::move(rse::engine::up_cast(rse::engine::snapshot_timedout(
			rse::engine::down_cast<rse::engine::state::noref_playing>(std::move(handle_)))));
	    }
	}
	next_tick += config_.tick_rate;
	if (TURBO_UNLIKELY(std::chrono::high_resolution_clock::now() > next_tick))
	{
	    std::time_t time = std::chrono::high_resolution_clock::to_time_t(next_tick);
	    LOG(WARNING) << "Tick at " << ctime(&time) << " missed";
	}
	++tick_count;
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

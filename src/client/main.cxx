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
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include <robocup2Dsim/common/metadata.hpp>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include "bot_io.hpp"
#include "config.hpp"
#include "event.hpp"
#include "event.hh"
#include "state.hpp"
#include "server_io.hpp"

namespace bmc = beam::message::capnproto;
namespace bme = beam::message;
namespace tip = turbo::ipc::posix;
namespace tpp = turbo::process::posix;
namespace rbc = robocup2Dsim::bcprotocol;
namespace rce = robocup2Dsim::common::entity;
namespace rcl = robocup2Dsim::client;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;
namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;

void parse_cmd_args(int argc, char* argv[], rcl::config& conf)
{
    kj::TopLevelProcessContext context(argv[0]);
    kj::MainFunc parse = kj::MainBuilder(
	    context,
	    "Robocup 2D Simulation Client v0.0",
	    "Launches bot and connects to the server")
	.addOption({'g', "goalie"}, [&] ()
	{
	    conf.goalie = true;
	    ++conf.bot_arg_offset;
	    return true;
	},
	"The bot is playing as the goalie.")
	.expectArg("address", [&] (kj::StringPtr arg)
	{
	    conf.address = arg.cStr();
	    ++conf.bot_arg_offset;
	    return true;
	})
	.expectArg("port", [&] (kj::StringPtr arg)
	{
	    int tmp = atoi(arg.cStr());
	    if (std::numeric_limits<decltype(conf.port)>::min() <= tmp && 
		    tmp <= std::numeric_limits<decltype(conf.port)>::max())
	    {
		conf.port = static_cast<decltype(conf.port)>(tmp);
		++conf.bot_arg_offset;
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
	.expectArg("team", [&] (kj::StringPtr arg)
	{
	    conf.team = arg.cStr();
	    ++conf.bot_arg_offset;
	    return true;
	})
	.expectArg("uniform", [&] (kj::StringPtr arg)
	{
	    try
	    {
		unsigned long tmp = std::stoul(arg.cStr());
		rce::UniformNumber uniform = rce::uint_to_uniform(tmp);
		conf.uniform = uniform;
		++conf.bot_arg_offset;
		return kj::MainBuilder::Validity(true);
	    }
	    catch (std::out_of_range&)
	    {
		return kj::MainBuilder::Validity(kj::str("uniform number must be in the range 1 to 11"));
	    }
	})
	.expectArg("bot_path", [&] (kj::StringPtr arg)
	{
	    conf.bot_path = arg.cStr();
	    ++conf.bot_arg_offset;
	    return true;
	})
	.expectZeroOrMoreArgs("bot_arg", [&] (kj::StringPtr)
	{
	    ++conf.bot_arg_count;
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

class client
{
public:
    client(const rcl::config& config, tpp::child&& bot);
    void run();
private:
    const rcl::config config_;
    rbc::bot_input_queue_type bot_input_queue_;
    rbc::bot_output_queue_type bot_output_queue_;
    rcs::client_status_queue_type client_status_queue_;
    rcs::client_trans_queue_type client_trans_queue_;
    rcs::server_status_queue_type server_status_queue_;
    rcs::server_trans_queue_type server_trans_queue_;
    rbc::bot_input_queue_type::producer& bot_input_producer_;
    rbc::bot_output_queue_type::consumer& bot_output_consumer_;
    rcs::client_status_queue_type::producer& client_status_producer_;
    rcs::client_trans_queue_type::producer& client_trans_producer_;
    rcs::server_status_queue_type::consumer& server_status_consumer_;
    rcs::server_trans_queue_type::consumer& server_trans_consumer_;
    rcl::event::basic_handle handle_;
    tip::signal_notifier notifier_;
    tpp::child bot_;
    rcl::bot_io bot_io_;
    rcl::server_io server_io_;
};

client::client(const rcl::config& config, tpp::child&& bot) :
	config_(config),
	bot_input_queue_(config_.bot_msg_queue_length),
	bot_output_queue_(config_.bot_msg_queue_length),
	client_status_queue_(config_.server_msg_queue_length),
	client_trans_queue_(config_.server_msg_queue_length),
	server_status_queue_(config_.server_msg_queue_length),
	server_trans_queue_(config_.server_msg_queue_length),
	bot_input_producer_(bot_input_queue_.get_producer()),
	bot_output_consumer_(bot_output_queue_.get_consumer()),
	client_status_producer_(client_status_queue_.get_producer()),
	client_trans_producer_(client_trans_queue_.get_producer()),
	server_status_consumer_(server_status_queue_.get_consumer()),
	server_trans_consumer_(server_trans_queue_.get_consumer()),
	handle_
	{
	    &bot_input_producer_,
	    &bot_output_consumer_,
	    &client_status_producer_,
	    &client_trans_producer_,
	    &server_status_consumer_,
	    &server_trans_consumer_,
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.bot_msg_word_length, config_.bot_msg_buffer_capacity))),
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.bot_msg_word_length, config_.bot_msg_buffer_capacity))),
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.server_msg_word_length, config_.server_msg_buffer_capacity))),
	    std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.server_msg_word_length, config_.server_msg_buffer_capacity))),
	    std::move(std::unique_ptr<rcl::client_game_state>()),
	    rcl::event::state::unregistered
	},
	notifier_(),
	bot_(std::move(bot)),
	bot_io_(config_, bot_.in, bot_.out, bot_input_queue_.get_consumer(), bot_output_queue_.get_producer()),
	server_io_(
		server_status_queue_.get_producer(),
		server_trans_queue_.get_producer(),
		client_status_queue_.get_consumer(),
		client_trans_queue_.get_consumer(),
		config_)
{
    // TODO: setup SIGCHLD handling
    std::unique_ptr<ren::physics> physics(new ren::physics());
    ren::register_system(rru::update_local_db(), std::move(physics));
}

void client::run()
{
    rcl::event::with(std::move(handle_),
	[&](rcl::event::handle<rcl::event::state::unregistered>&& handle)
	{
	    handle_ = std::move(rcl::event::spawned(std::move(handle), config_));
	}
    );
    bmc::payload<rbc::BotOutput> bot_payload;
    bmc::payload<rcs::ServerStatus> status_payload;
    bmc::payload<rcs::ServerTransaction> trans_payload;
    std::chrono:: high_resolution_clock::time_point next_frame_time = std::chrono::high_resolution_clock::now() + config_.frame_duration;
    asio::io_service service;
    asio::high_resolution_timer timer(service);
    rco::frame_number frame = 0U;
    bool should_run = true;
    while (should_run)
    {
	timer.expires_at(next_frame_time);
	timer.wait();
	if (bot_output_consumer_.try_dequeue_move(bot_payload) == rbc::bot_output_queue_type::consumer::result::success)
	{
	    bmc::statement<rbc::BotOutput> bot_output(std::move(bot_payload));
	    rcl::event::with(std::move(handle_),
		[&](rcl::event::handle<rcl::event::state::playing>&& handle)
		{
		    if (bot_output.read().isAction())
		    {
			handle_ = std::move(rcl::event::control_actioned(std::move(handle), bot_output.read().getAction()));
		    }
		    else if (bot_output.read().isQuery())
		    {
			handle_ = std::move(rcl::event::query_requested(std::move(handle), bot_output.read().getQuery()));
		    }
		    else if (bot_output.read().isCrash())
		    {
			handle_ = std::move(rcl::event::bot_crashed(std::move(handle)));
		    }
		},
		[&](rcl::event::handle<rcl::event::state::onbench>&& handle)
		{
		    if (bot_output.read().isQuery())
		    {
			handle_ = std::move(rcl::event::query_requested(std::move(handle), bot_output.read().getQuery()));
		    }
		    else if (bot_output.read().isCrash())
		    {
			handle_ = std::move(rcl::event::bot_crashed(std::move(handle)));
		    }
		},
		[&](rcl::event::handle<rcl::event::state::unregistered>&& handle)
		{
		    if (bot_output.read().isShutDown())
		    {
			handle_ = std::move(rcl::event::bot_terminated(std::move(handle)));
		    }
		    else if (bot_output.read().isCrash())
		    {
			handle_ = std::move(rcl::event::bot_crashed(std::move(handle)));
		    }
		}
	    );
	}
	if (server_status_consumer_.try_dequeue_move(status_payload) == rcs::server_status_queue_type::consumer::result::success)
	{
	    bmc::statement<rcs::ServerStatus> server_status(std::move(status_payload));
	    rcl::event::with(std::move(handle_),
		[&](rcl::event::handle<rcl::event::state::onbench>&& handle)
		{
                    if (server_status.read().isPing())
                    {
                        handle_ = std::move(rcl::event::received_ping(
                                std::move(handle),
                                server_status.read().getValue1().getAs<rcs::Ping>()));
                    }
		},
		[&](rcl::event::handle<rcl::event::state::playing>&& handle)
		{
		    handle_ = std::move(rcl::event::received_snapshot(std::move(handle), server_status.read()));
		}
	    );
	}
	if (server_trans_consumer_.try_dequeue_move(trans_payload) == rcs::server_trans_queue_type::consumer::result::success)
	{
	    bmc::statement<rcs::ServerTransaction> server_trans(std::move(trans_payload));
	    rcl::event::with(std::move(handle_),
		[&](rcl::event::handle<rcl::event::state::playing>&& handle)
		{
		    if (server_trans.read().isPlayJudgement())
		    {
			handle_ = std::move(rcl::event::play_judged(std::move(handle), server_trans.read().getPlayJudgement()));
		    }
		    else if (server_trans.read().isMatchClose())
		    {
			handle_ = std::move(rcl::event::match_closed(std::move(handle), server_trans.read().getMatchClose()));
		    }
		    else if (server_trans.read().isMatchAbort())
		    {
			handle_ = std::move(rcl::event::match_aborted(std::move(handle), server_trans.read().getMatchAbort()));
		    }
		    else if (server_trans.read().isDisconnect())
		    {
			handle_ = std::move(rcl::event::disconnected(std::move(handle)));
		    }
		},
		[&](rcl::event::handle<rcl::event::state::onbench>&& handle)
		{
		    if (server_trans.read().isFieldOpen())
		    {
			handle_ = std::move(rcl::event::field_opened(std::move(handle), server_trans.read().getFieldOpen()));
		    }
		    else if (server_trans.read().isMatchAbort())
		    {
			handle_ = std::move(rcl::event::match_aborted(std::move(handle), server_trans.read().getMatchAbort()));
		    }
		    else if (server_trans.read().isDisconnect())
		    {
			handle_ = std::move(rcl::event::disconnected(std::move(handle)));
		    }
		},
		[&](rcl::event::handle<rcl::event::state::onbench>&& handle)
		{
		    if (server_trans.read().isDisconnect())
		    {
			handle_ = std::move(rcl::event::disconnected(std::move(handle)));
		    }
		    // TODO: should handle spawn event?
		},
		[&](rcl::event::handle<rcl::event::state::unregistered>&& handle)
		{
		    if (server_trans.read().isRegAck())
		    {
			handle_ = std::move(rcl::event::registration_succeeded(
                                std::move(handle),
                                server_trans.read().getRegAck()));
		    }
		    else if (server_trans.read().isRegError())
		    {
			handle_ = std::move(rcl::event::registration_failed(std::move(handle), server_trans.read().getRegError()));
		    }
		}
	    );
	}
	rcl::event::with(std::move(handle_),
	    [&](rcl::event::handle<rcl::event::state::playing>&& handle)
	    {
		if (frame % config_.simulation_frequency == config_.simulation_start_frame)
		{
		    handle_ = std::move(rcl::event::simulation_timedout(std::move(handle)));
		}
		if (frame % config_.sensor_frequency == config_.sensor_start_frame)
		{
		    handle_ = std::move(rcl::event::sensor_timedout(std::move(handle)));
		}
		if (frame % config_.upload_frequency == config_.upload_start_frame)
		{
		    handle_ = std::move(rcl::event::upload_timedout(std::move(handle)));
		}
	    }
	);
	next_frame_time += config_.frame_duration;
	if (TURBO_UNLIKELY(std::chrono::high_resolution_clock::now() > next_frame_time))
	{
	    std::time_t time = std::chrono::high_resolution_clock::to_time_t(next_frame_time);
	    LOG(WARNING) << "Frame number " << frame << " at " << ctime(&time) << " missed";
	}
	++frame;
    }
}

int main(int argc, char* argv[])
{
    rcl::config conf;
    parse_cmd_args(argc, argv, conf);
    tpp::child bot = tpp::spawn(conf.bot_path.c_str(), &argv[conf.bot_arg_offset], {}, 2 << 16);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = (kj::_::Debug::shouldLog(kj::_::Debug::Severity::INFO)) ? 0 : 1;
    google::InstallFailureSignalHandler();
    client cl(std::move(conf), std::move(bot));
    cl.run();
    return 0;
}

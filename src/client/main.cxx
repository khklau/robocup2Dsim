#include <cstdlib>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <beam/message/capnproto.hxx>
#include <kj/common.h>
#include <kj/debug.h>
#include <kj/main.h>
#include <kj/string.h>
#include <glog/logging.h>
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <turbo/container/spsc_ring_queue.hxx>
#include <turbo/ipc/posix/pipe.hpp>
#include <turbo/ipc/posix/signal_notifier.hpp>
#include <turbo/process/posix/spawn.hpp>
#include "bot_io.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "engine.hxx"
#include "server_io.hpp"

namespace bme = beam::message;
namespace tip = turbo::ipc::posix;
namespace tpp = turbo::process::posix;
namespace rbc = robocup2Dsim::bcprotocol;
namespace rcl = robocup2Dsim::client;
namespace rcs = robocup2Dsim::csprotocol;

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
	    int tmp = atoi(arg.cStr());
	    if (std::numeric_limits<decltype(conf.uniform)>::min() <= tmp && 
		    tmp <= std::numeric_limits<decltype(conf.uniform)>::max())
	    {
		conf.uniform = static_cast<decltype(conf.uniform)>(tmp);
		++conf.bot_arg_offset;
		return kj::MainBuilder::Validity(true);
	    }
	    else
	    {
		return kj::MainBuilder::Validity(kj::str(
			"uniform number must be in the range ",
			std::numeric_limits<decltype(conf.uniform)>::min(),
			" to ",
			std::numeric_limits<decltype(conf.uniform)>::max()));
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
    client(const rcl::config&& config, tpp::child&& bot);
    void run();
private:
    const rcl::config config_;
    rcl::engine::basic_handle handle_;
    tip::signal_notifier notifier_;
    tpp::child&& bot_;
    rcl::bot_io bot_io_;
    rcl::server_io server_io_;
};

client::client(const rcl::config&& config, tpp::child&& bot) :
	config_(std::move(config)),
	handle_
	{
	    std::move(std::unique_ptr<rbc::bot_input_queue_type>(new rbc::bot_input_queue_type(config_.bot_msg_queue_length))),
	    std::move(std::unique_ptr<rbc::bot_output_queue_type>(new rbc::bot_output_queue_type(config_.bot_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::client_status_queue_type>(new rcs::client_status_queue_type(config_.server_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::client_trans_queue_type>(new rcs::client_trans_queue_type(config_.server_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::server_status_queue_type>(new rcs::server_status_queue_type(config_.server_msg_queue_length))),
	    std::move(std::unique_ptr<rcs::server_trans_queue_type>(new rcs::server_trans_queue_type(config_.server_msg_queue_length))),
	    rcl::engine::state::withbot_unregistered
	},
	notifier_(),
	bot_(std::move(bot)),
	bot_io_(bot_.in, bot_.out, handle_.bot_input_queue->get_consumer(), handle_.bot_output_queue->get_producer()),
	server_io_(
		handle_.server_status_queue->get_producer(),
		handle_.server_trans_queue->get_producer(),
		handle_.client_status_queue->get_consumer(),
		handle_.client_trans_queue->get_consumer(),
		config_)
{
    // TODO: setup SIGCHLD handling
}

void client::run()
{
    std::unique_ptr<bme::capnproto<rbc::BotOutput>> bot_output;
    std::unique_ptr<bme::capnproto<rcs::ServerStatus>> server_status;
    std::unique_ptr<bme::capnproto<rcs::ServerTransaction>> server_trans;
    bool should_run = true;
    while (should_run)
    {
	if (handle_.bot_output_queue->get_consumer().try_dequeue_move(bot_output) == rbc::bot_output_queue_type::consumer::result::success)
	{
	    if (handle_.engine_state == rcl::engine::state::withbot_playing && bot_output->get_reader().isControl())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::control_actioned(
			rcl::engine::down_cast<rcl::engine::state::withbot_playing>(std::move(handle_)),
			bot_output->get_reader().getControl())));
	    }
	    else if (handle_.engine_state == rcl::engine::state::withbot_playing && bot_output->get_reader().isQuery())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::query_requested(
			rcl::engine::down_cast<rcl::engine::state::withbot_playing>(std::move(handle_)),
			bot_output->get_reader().getQuery())));
	    }
	    else if (handle_.engine_state == rcl::engine::state::withbot_onbench && bot_output->get_reader().isQuery())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::query_requested(
			rcl::engine::down_cast<rcl::engine::state::withbot_onbench>(std::move(handle_)),
			bot_output->get_reader().getQuery())));
	    }
	}
	if (handle_.server_status_queue->get_consumer().try_dequeue_move(server_status) == rcs::server_status_queue_type::consumer::result::success)
	{
	    if (handle_.engine_state == rcl::engine::state::withbot_playing)
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::received_snapshot(
			rcl::engine::down_cast<rcl::engine::state::withbot_playing>(std::move(handle_)),
			server_status->get_reader())));
	    }
	}
	if (handle_.server_trans_queue->get_consumer().try_dequeue_move(server_trans) == rcs::server_trans_queue_type::consumer::result::success)
	{
	    if (handle_.engine_state == rcl::engine::state::withbot_playing && server_trans->get_reader().isPlayJudgement())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::play_judged(
			rcl::engine::down_cast<rcl::engine::state::withbot_playing>(std::move(handle_)),
			server_trans->get_reader().getPlayJudgement())));
	    }
	    else if (handle_.engine_state == rcl::engine::state::withbot_onbench && server_trans->get_reader().isFieldOpen())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::field_opened(
			rcl::engine::down_cast<rcl::engine::state::withbot_onbench>(std::move(handle_)),
			server_trans->get_reader().getFieldOpen())));
	    }
	    else if (handle_.engine_state == rcl::engine::state::withbot_onbench && server_trans->get_reader().isMatchAbort())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::match_aborted(
			rcl::engine::down_cast<rcl::engine::state::withbot_onbench>(std::move(handle_)),
			server_trans->get_reader().getMatchAbort())));
	    }
	    else if (handle_.engine_state == rcl::engine::state::withbot_unregistered && server_trans->get_reader().isRegSuccess())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::registration_succeeded(
			rcl::engine::down_cast<rcl::engine::state::withbot_unregistered>(std::move(handle_)))));
	    }
	    else if (handle_.engine_state == rcl::engine::state::withbot_unregistered && server_trans->get_reader().isRegError())
	    {
		handle_ = std::move(rcl::engine::up_cast(rcl::engine::registration_failed(
			rcl::engine::down_cast<rcl::engine::state::withbot_unregistered>(std::move(handle_)),
			server_trans->get_reader().getRegError())));
	    }
	}

    }
}

int main(int argc, char* argv[])
{
    rcl::config conf;
    parse_cmd_args(argc, argv, conf);
    tpp::child&& bot = tpp::spawn(conf.bot_path.c_str(), &argv[conf.bot_arg_offset], {}, 2 << 16);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = (kj::_::Debug::shouldLog(kj::_::Debug::Severity::INFO)) ? 0 : 1;
    google::InstallFailureSignalHandler();
    client cl(std::move(conf), std::move(bot));
    cl.run();
    return 0;
}

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

namespace tip = turbo::ipc::posix;
namespace tpp = turbo::process::posix;
namespace rc = robocup2Dsim::client;

void parse_cmd_args(int argc, char* argv[], rc::config& conf)
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
    client(const rc::config&& config, turbo::process::posix::child&& bot);
    void run();
private:
    const rc::config config_;
    rc::handle<rc::state::withbot_unregistered> handle_;
    tip::signal_notifier notifier_;
    turbo::process::posix::child&& bot_;
    rc::bot_io bot_io_;
    rc::server_io server_io_;
};

client::client(const rc::config&& config, turbo::process::posix::child&& bot) :
	config_(std::move(config)),
	handle_
	{
	    std::move(std::unique_ptr<robocup2Dsim::bcprotocol::bot_input_queue_type>(new robocup2Dsim::bcprotocol::bot_input_queue_type(config_.bot_msg_queue_length))),
	    std::move(std::unique_ptr<robocup2Dsim::bcprotocol::bot_output_queue_type>(new robocup2Dsim::bcprotocol::bot_output_queue_type(config_.bot_msg_queue_length))),
	    std::move(std::unique_ptr<robocup2Dsim::csprotocol::client_status_queue_type>(new robocup2Dsim::csprotocol::client_status_queue_type(config_.server_msg_queue_length))),
	    std::move(std::unique_ptr<robocup2Dsim::csprotocol::client_trans_queue_type>(new robocup2Dsim::csprotocol::client_trans_queue_type(config_.server_msg_queue_length))),
	    std::move(std::unique_ptr<robocup2Dsim::csprotocol::server_status_queue_type>(new robocup2Dsim::csprotocol::server_status_queue_type(config_.server_msg_queue_length))),
	    std::move(std::unique_ptr<robocup2Dsim::csprotocol::server_trans_queue_type>(new robocup2Dsim::csprotocol::server_trans_queue_type(config_.server_msg_queue_length)))
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
}

int main(int argc, char* argv[])
{
    rc::config conf;
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

#include <cstdlib>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <kj/common.h>
#include <kj/debug.h>
#include <kj/main.h>
#include <kj/string.h>
#include <glog/logging.h>
#include <turbo/ipc/posix/pipe.hpp>
#include <turbo/process/posix/spawn.hpp>
#include "config.hpp"
#include "state_machine.hpp"

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

int main(int argc, char* argv[])
{
    rc::config conf;
    parse_cmd_args(argc, argv, conf);
    tpp::child&& bot = tpp::spawn(conf.bot_path.c_str(), &argv[conf.bot_arg_offset], {}, 2 << 16);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = (kj::_::Debug::shouldLog(kj::_::Debug::Severity::INFO)) ? 0 : 1;
    google::InstallFailureSignalHandler();
    rc::state_machine machine(conf, std::move(bot));
    return 0;
}

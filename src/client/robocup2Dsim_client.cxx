#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <kj/common.h>
#include <kj/debug.h>
#include <kj/main.h>
#include <kj/string.h>
#include <glog/logging.h>
#include <turbo/ipc/posix/pipe.hpp>
#include <turbo/process/posix/spawn.hpp>

namespace tpp = turbo::process::posix;

struct config
{
    config() : port(0) { }
    std::string address;
    int port;
    std::string bot_path;
    std::vector<std::string> bot_arg_list;
};

int main(int argc, char* argv[])
{
    config conf;
    kj::TopLevelProcessContext context(argv[0]);
    kj::MainFunc parse = kj::MainBuilder(
	    context,
	    "Robocup 2D Simulation Client v0.0",
	    "Launches bot and connects to the server")
	.expectArg("address", [&] (kj::StringPtr arg) { conf.address = arg.cStr(); return true; })
	.expectArg("port", [&] (kj::StringPtr arg) { conf.port = atoi(arg.cStr()); return true; })
	.expectArg("bot_path", [&] (kj::StringPtr arg) { conf.bot_path = arg.cStr(); return true; })
	.expectZeroOrMoreArgs("bot_arg", [&] (kj::StringPtr arg) { conf.bot_arg_list.push_back(arg.cStr()); return true; })
	.build();
    KJ_STACK_ARRAY(kj::StringPtr, params, argc - 1, 8, 32);
    for (int iter = 1; iter < argc; ++iter)
    {
	params[iter - 1] = argv[iter];
    }
    parse(argv[0], params);
    if (kj::_::Debug::shouldLog(kj::_::Debug::Severity::INFO))
    {
    }
    tpp::child&& bot = tpp::spawn(conf.bot_path.c_str(), &argv[4], {}, 2 << 16);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    return 0;
}

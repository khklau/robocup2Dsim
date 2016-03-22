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
#include <turbo/ipc/posix/signal_notifier.hpp>
#include <turbo/process/posix/spawn.hpp>
#include "client_io.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "engine.hxx"
#include "ref_io.hpp"

namespace tip = turbo::ipc::posix;
namespace tpp = turbo::process::posix;
namespace rse = robocup2Dsim::server;

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
    server(const rse::config&& config, tpp::child&& ref);
    void run();
private:
    const rse::config config_;
    rse::engine::basic_handle handle_;
    tip::signal_notifier notifier_;
    tpp::child&& ref_;
    rse::ref_io ref_io_;
    rse::client_io client_io_;
};

void server::run()
{
}

int main(int argc, char* argv[])
{
    rse::config conf;
    parse_cmd_args(argc, argv, conf);
    tpp::child&& ref = tpp::spawn(conf.ref_path.c_str(), &argv[conf.ref_arg_offset], {}, 2 << 16);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = (kj::_::Debug::shouldLog(kj::_::Debug::Severity::INFO)) ? 0 : 1;
    google::InstallFailureSignalHandler();
    //rse::state_machine machine(conf, std::move(ref));
    return 0;
}

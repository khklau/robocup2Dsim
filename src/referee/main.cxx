#include <utility>
#include <kj/common.h>
#include <kj/debug.h>
#include <kj/main.h>
#include <kj/string.h>
#include "config.hpp"
#include "referee.hpp"
#include "server_io.hpp"
#include "server_io.hh"

namespace bmc = beam::message::capnproto;
namespace rre = robocup2Dsim::referee;
namespace rsr = robocup2Dsim::srprotocol;

void parse_cmd_args(int argc, char* argv[], rre::config& conf)
{
    kj::TopLevelProcessContext context(argv[0]);
    kj::MainFunc parse = kj::MainBuilder(
	    context,
	    "Robocup 2D Simulation Server v0.0",
	    "Launches ref and waits for clients to connect")
	.expectArg("msg_word_length", [&] (kj::StringPtr arg)
	{
	    int tmp = atoi(arg.cStr());
	    if (std::numeric_limits<decltype(conf.msg_word_length)>::min() <= tmp && 
		    tmp <= std::numeric_limits<decltype(conf.msg_word_length)>::max())
	    {
		conf.msg_word_length = static_cast<decltype(conf.msg_word_length)>(tmp);
		return kj::MainBuilder::Validity(true);
	    }
	    else
	    {
		return kj::MainBuilder::Validity(kj::str(
			"length must be int the range ",
			std::numeric_limits<decltype(conf.msg_word_length)>::min(),
			" to ",
			std::numeric_limits<decltype(conf.msg_word_length)>::max()));
	    }
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
    rre::config config;
    parse_cmd_args(argc, argv, config);
    rre::server_io io(config);
    rre::referee ref(config);
    io.run([&](std::queue<bmc::payload<rsr::RefInput>>& in, std::queue<bmc::payload<rsr::RefOutput>>& out) -> void
    {
	ref.process(in, out);
    });
    return 0;
}

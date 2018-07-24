#include "referee.hpp"

namespace bmc = beam::message::capnproto;
namespace rcc = robocup2Dsim::common::command;
namespace rre = robocup2Dsim::referee;
namespace rse = robocup2Dsim::server;
namespace rsr = robocup2Dsim::srprotocol;

namespace robocup2Dsim {
namespace referee {

referee::referee(const config& conf)
    :
	config_(conf),
	enrollment_(new rse::enrollment()),
	roster_()
{ }

void referee::process(std::queue<bmc::payload<rsr::RefInput>>& in, std::queue<bmc::payload<rsr::RefOutput>>& out)
{
    while (0U < in.size())
    {
	bmc::statement<rsr::RefInput> statement(std::move(in.front()));
	in.pop();

	rsr::RefInput::Reader input = statement.read();
	switch (input.which())
	{
	    case rsr::RefInput::RULES:
		break;
	    case rsr::RefInput::REGISTRATION:
		process(input.getRegistration(), out);
		break;
	}
    }
}

void referee::process(
	const rsr::RegistrationRequest::Reader& registration,
	std::queue<bmc::payload<rsr::RefOutput>>& out)
{
}

} // namespace referee
} // namespace robocup2Dsim

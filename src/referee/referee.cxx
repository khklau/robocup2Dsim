#include "referee.hpp"

namespace bmc = beam::message::capnproto;
namespace rre = robocup2Dsim::referee;
namespace rsr = robocup2Dsim::srprotocol;

namespace robocup2Dsim {
namespace referee {

referee::referee(const config& conf)
    :
	config_(conf)
{ }

void referee::process(std::queue<bmc::payload<rsr::RefInput>>& in, std::queue<bmc::payload<rsr::RefOutput>>& out)
{
}

} // namespace referee
} // namespace robocup2Dsim

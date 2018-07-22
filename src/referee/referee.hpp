#ifndef ROBOCUP2DSIM_REFEREE_REFEREE_HPP
#define ROBOCUP2DSIM_REFEREE_REFEREE_HPP

#include "config.hpp"

#include <beam/message/capnproto.hh>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <robocup2Dsim/srprotocol/protocol.hpp>

#include <queue>

namespace robocup2Dsim {
namespace referee {

class referee
{
public:
    referee(const config& conf);
    void process(
	    std::queue<beam::message::capnproto::payload<robocup2Dsim::srprotocol::RefInput>>& in,
	    std::queue<beam::message::capnproto::payload<robocup2Dsim::srprotocol::RefOutput>>& out);
private:
    config config_;
};

} // namespace referee
} // namespace robocup2Dsim

#endif

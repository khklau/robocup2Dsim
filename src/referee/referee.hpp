#ifndef ROBOCUP2DSIM_REFEREE_REFEREE_HPP
#define ROBOCUP2DSIM_REFEREE_REFEREE_HPP

#include "config.hpp"

#include <beam/message/buffer_pool.hpp>
#include <beam/message/capnproto.hh>
#include <robocup2Dsim/common/command.capnp.h>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <robocup2Dsim/srprotocol/protocol.hpp>
#include <robocup2Dsim/server/roster.hpp>
#include <robocup2Dsim/server/state.hpp>

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
    void process(
	    const robocup2Dsim::srprotocol::RegistrationRequest::Reader& registration,
	    std::queue<beam::message::capnproto::payload<robocup2Dsim::srprotocol::RefOutput>>& out);
private:
    config config_;
    std::unique_ptr<robocup2Dsim::server::enrollment> enrollment_;
    std::unique_ptr<robocup2Dsim::server::roster> roster_;
    std::unique_ptr<robocup2Dsim::server::server_game_state> game_state_;
    beam::message::buffer_pool pool_;
};

} // namespace referee
} // namespace robocup2Dsim

#endif

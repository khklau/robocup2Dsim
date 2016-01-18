#ifndef ROBOCUP2DSIM_BCPROTOCOL_PROTOCOL_HPP
#define ROBOCUP2DSIM_BCPROTOCOL_PROTOCOL_HPP

#include <memory>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace bcprotocol {

typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::bcprotocol::ClientTransmission>>> client_trans_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::bcprotocol::BotTransmission>>> bot_trans_queue_type;

} // namespace bcprotocol
} // namespace robocup2Dsim

#endif

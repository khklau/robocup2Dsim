#ifndef ROBOCUP2DSIM_CLIENT_SPECIFICATION_HPP
#define ROBOCUP2DSIM_CLIENT_SPECIFICATION_HPP

#include <memory>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace client {

typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::bcprotocol::ClientTransmission>>> bot_in_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::bcprotocol::BotTransmission>>> bot_out_queue_type;

typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ClientTransmission>>> server_in_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerTransmission>>> server_out_queue_type;

} // namespace client
} // namespace robocup2Dsim

#endif

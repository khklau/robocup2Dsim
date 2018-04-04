#ifndef ROBOCUP2DSIM_BCPROTOCOL_PROTOCOL_HPP
#define ROBOCUP2DSIM_BCPROTOCOL_PROTOCOL_HPP

#include <memory>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace bcprotocol {

typedef beam::message::capnproto::payload<robocup2Dsim::bcprotocol::BotInput> input_msg_type;
typedef beam::message::capnproto::payload<robocup2Dsim::bcprotocol::BotOutput> output_msg_type;

typedef turbo::container::spsc_ring_queue<input_msg_type> bot_input_queue_type;
typedef turbo::container::spsc_ring_queue<output_msg_type> bot_output_queue_type;

} // namespace bcprotocol
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_SRPROTOCOL_PROTOCOL_HPP
#define ROBOCUP2DSIM_SRPROTOCOL_PROTOCOL_HPP

#include <memory>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace srprotocol {

typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::srprotocol::RefInput>>> ref_input_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::srprotocol::RefOutput>>> ref_output_queue_type;

} // namespace srprotocol
} // namespace robocup2Dsim

#endif

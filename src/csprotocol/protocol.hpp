#ifndef ROBOCUP2DSIM_CSPROTOCOL_PROTOCOL_HPP
#define ROBOCUP2DSIM_CSPROTOCOL_PROTOCOL_HPP

#include <limits>
#include <memory>
#include <beam/duplex/common.hpp>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace csprotocol {

typedef turbo::container::spsc_ring_queue<beam::message::capnproto::payload<robocup2Dsim::csprotocol::ClientStatus>> client_status_queue_type;
typedef turbo::container::spsc_ring_queue<beam::message::capnproto::payload<robocup2Dsim::csprotocol::ClientTransaction>> client_trans_queue_type;
typedef turbo::container::spsc_ring_queue<beam::message::capnproto::payload<robocup2Dsim::csprotocol::ServerStatus>> server_status_queue_type;
typedef turbo::container::spsc_ring_queue<beam::message::capnproto::payload<robocup2Dsim::csprotocol::ServerTransaction>> server_trans_queue_type;

} // namespace csprotocol
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_CSPROTOCOL_PROTOCOL_HPP
#define ROBOCUP2DSIM_CSPROTOCOL_PROTOCOL_HPP

#include <memory>
#include <beam/duplex/common.hpp>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace csprotocol {

typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ClientStatus>>> client_status_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ClientTransaction>>> client_trans_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerStatus>>> server_status_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerTransaction>>> server_trans_queue_type;

typedef std::uint64_t recipient_id;

inline beam::duplex::common::endpoint_id convert(const recipient_id& recipient)
{
    beam::duplex::common::endpoint_id endpoint{
	    static_cast<decltype(endpoint.address)>(recipient >> std::numeric_limits<decltype(endpoint.address)>::digits),
	    static_cast<decltype(endpoint.port)>(recipient & std::numeric_limits<decltype(endpoint.port)>::max())};
    return endpoint;
}

inline recipient_id convert(const beam::duplex::common::endpoint_id& endpoint)
{
    recipient_id recipient = endpoint.address;
    recipient = (recipient << std::numeric_limits<decltype(endpoint.address)>::digits) + endpoint.port;
    return recipient;
}

} // namespace csprotocol
} // namespace robocup2Dsim

#endif

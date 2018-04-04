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

typedef std::uint64_t client_id;

static const client_id no_client = 0U;

inline beam::duplex::common::endpoint_id convert(const client_id& recipient)
{
    beam::duplex::common::endpoint_id endpoint{
	    static_cast<decltype(endpoint.address)>(recipient >> std::numeric_limits<decltype(endpoint.address)>::digits),
	    static_cast<decltype(endpoint.port)>(recipient & std::numeric_limits<decltype(endpoint.port)>::max())};
    return endpoint;
}

inline client_id convert(const beam::duplex::common::endpoint_id& endpoint)
{
    client_id recipient = endpoint.address;
    recipient = (recipient << std::numeric_limits<decltype(endpoint.address)>::digits) + endpoint.port;
    return recipient;
}

} // namespace csprotocol
} // namespace robocup2Dsim

#endif

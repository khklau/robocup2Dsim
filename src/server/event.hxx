#ifndef ROBOCUP2DSIM_SERVER_EVENT_HXX
#define ROBOCUP2DSIM_SERVER_EVENT_HXX

#include "event.hpp"
#include <beam/message/capnproto.hxx>
#include <turbo/container/spsc_ring_queue.hxx>

namespace robocup2Dsim {
namespace server {
namespace event {

template <state state_value>
handle<state_value>::handle(
	decltype(ref_input_producer) ref_in,
	decltype(ref_output_consumer) ref_out,
	decltype(server_status_producer) server_status,
	decltype(server_trans_producer) server_trans,
	decltype(client_status_consumer) client_status,
	decltype(client_trans_consumer) client_trans,
	decltype(ref_inbound_buffer_pool)&& ref_inbound_pool,
	decltype(ref_outbound_buffer_pool)&& ref_outbound_pool,
	decltype(server_outbound_buffer_pool)&& server_outbound_pool,
	decltype(client_inbound_buffer_pool)&& client_inbound_pool)
    :
	basic_handle(
		ref_in,
		ref_out,
		server_status,
		server_trans,
		client_status,
		client_trans,
		std::move(ref_inbound_pool),
		std::move(ref_outbound_pool),
		std::move(server_outbound_pool),
		std::move(client_inbound_pool),
		state_value)
{ }

template <state state_value>
template <state other_value>
handle<state_value>::handle(handle<other_value>&& other)
    :
	basic_handle(std::move(static_cast<basic_handle&&>(other)))
{
    server_state = state_value;
}

} // namespace event
} // namespace server
} // namespace robocup2Dsim

#endif

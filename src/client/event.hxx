#ifndef ROBOCUP2DSIM_CLIENT_EVENT_HXX
#define ROBOCUP2DSIM_CLIENT_EVENT_HXX

#include "event.hpp"
#include <beam/message/capnproto.hxx>
#include <turbo/container/spsc_ring_queue.hxx>

namespace robocup2Dsim {
namespace client {
namespace event {

template <state state_value>
handle<state_value>::handle(
	decltype(bot_input_producer) bot_in,
	decltype(bot_output_consumer) bot_out,
	decltype(client_status_producer) client_status,
	decltype(client_trans_producer) client_trans,
	decltype(server_status_consumer) server_status,
	decltype(server_trans_consumer) server_trans,
	decltype(bot_inbound_buffer_pool)&& bot_inbound_pool,
	decltype(bot_outbound_buffer_pool)&& bot_outbound_pool,
	decltype(client_outbound_buffer_pool)&& client_outbound_pool,
	decltype(server_inbound_buffer_pool)&& server_inbound_pool)
    :
	basic_handle(
		bot_in,
		bot_out,
		client_status,
		client_trans,
		server_status,
		server_trans,
		std::move(bot_inbound_pool),
		std::move(bot_outbound_pool),
		std::move(client_outbound_pool),
		std::move(server_inbound_pool),
		state_value)
{ }

template <state state_value>
template <state other_value>
handle<state_value>::handle(handle<other_value>&& other)
    :
	basic_handle(std::move(static_cast<basic_handle&&>(other)))
{
    client_state = state_value;
}

} // namespace event
} // namespace client
} // namespace robocup2Dsim

#endif

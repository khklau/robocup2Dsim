#ifndef ROBOCUP2DSIM_CLIENT_EVENT_HXX
#define ROBOCUP2DSIM_CLIENT_EVENT_HXX

#include "event.hpp"
#include <beam/message/capnproto.hxx>
#include <turbo/container/spsc_ring_queue.hxx>
#include <turbo/type_utility/function_traits.hpp>

namespace robocup2Dsim {
namespace client {
namespace event {

template <state state_value>
basic_handle& basic_handle::operator=(handle<state_value>&& other)
{
    (*this) = static_cast<basic_handle&&>(std::move(other));
    client_state = state_value;
    return *this;
}

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
	basic_handle(static_cast<basic_handle&&>(other))
{
    client_state = state_value;
}

namespace detail {

namespace ttu = turbo::type_utility;

template <typename func_t>
using first_arg_type = typename std::remove_reference<
	typename std::tuple_element<0U,typename ttu::function_traits<func_t>::arg_types>::type
	>::type;

template <class handle_t>
struct handle_traits
{
};

template <state state_value>
struct handle_traits<::robocup2Dsim::client::event::handle<state_value>>
{
    static const state value = state_value;
};

} // namespace detail

template <typename func1_t, typename... funcn_t>
inline void with(basic_handle&& arg, func1_t&& head_func, funcn_t&&... tail_funcs)
{
    typedef detail::first_arg_type<func1_t> handle_arg_type;
    if (arg.client_state == detail::handle_traits<handle_arg_type>::value)
    {
	head_func(static_cast<handle_arg_type&&>(std::move(arg)));
    }
    else
    {
	with<funcn_t...>(std::move(arg), std::forward<funcn_t&&>(tail_funcs)...);
    }
}

template <typename func_t>
inline void with(basic_handle&& arg, func_t&& func)
{
    typedef detail::first_arg_type<func_t> handle_arg_type;
    if (arg.client_state == detail::handle_traits<handle_arg_type>::value)
    {
	func(static_cast<handle_arg_type&&>(std::move(arg)));
    }
}

} // namespace event
} // namespace client
} // namespace robocup2Dsim

#endif

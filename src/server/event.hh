#ifndef ROBOCUP2DSIM_SERVER_EVENT_HXX
#define ROBOCUP2DSIM_SERVER_EVENT_HXX

#include "event.hpp"
#include <beam/message/capnproto.hh>
#include <turbo/algorithm/recovery.hh>
#include <turbo/container/spsc_ring_queue.hh>
#include <turbo/type_utility/function_traits.hpp>

namespace robocup2Dsim {
namespace server {
namespace event {

template <state state_value>
basic_handle& basic_handle::operator=(handle<state_value>&& other)
{
    (*this) = static_cast<basic_handle&&>(std::move(other));
    server_state = state_value;
    return *this;
}

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

namespace detail {

namespace bin = beam::internet;
namespace bmc = beam::message::capnproto;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;
namespace tar = turbo::algorithm::recovery;
namespace ttu = turbo::type_utility;

template <state next_state, state current_state>
handle<next_state> registration_requested(
	handle<current_state>&& input,
	bin::endpoint_id client,
	const rcs::RegistrationRequest::Reader& request)
{
    bmc::form<rcs::ServerTransaction> form(std::move(input.server_outbound_buffer_pool->borrow()), client);
    rcs::ServerTransaction::Builder trans = form.build();
    switch (input.enrollment->register_client(client, request.getDetails()))
    {
	case enrollment::register_result::version_mismatch:
	{
	    rcs::RegistrationError::Builder error = trans.initRegError();
	    error.initVersionMismatch();
	    break;
	}
	case enrollment::register_result::team_slot_taken:
	{
	    rcs::RegistrationError::Builder error = trans.initRegError();
	    error.initTeamTaken();
	    break;
	}
	case enrollment::register_result::uniform_taken:
	{
	    rcs::RegistrationError::Builder error = trans.initRegError();
	    error.initUniformTaken();
	    break;
	}
	case enrollment::register_result::goalie_taken:
	{
	    rcs::RegistrationError::Builder error = trans.initRegError();
	    error.initGoalieTaken();
	    break;
	}
	case enrollment::register_result::success:
	default:
	{
	    trans.setRegSuccess();
	    break;
	}
    }
    bmc::payload<rcs::ServerTransaction> payload(std::move(bmc::serialise(*(input.server_outbound_buffer_pool), form)));
    tar::retry_with_random_backoff([&]()
    {
	if (input.server_trans_producer->try_enqueue_move(std::move(payload)) == rcs::server_trans_queue_type::producer::result::success)
	{
	    return tar::try_state::done;
	}
	else
	{
	    return tar::try_state::retry;
	}
    });
    handle<next_state> output(std::move(input));
    return std::move(output);
}

template <typename func_t>
using first_arg_type = typename std::remove_reference<
	typename std::tuple_element<0U,typename ttu::function_traits<func_t>::arg_types>::type
	>::type;

template <class handle_t>
struct handle_traits
{
};

template <state state_value>
struct handle_traits<::robocup2Dsim::server::event::handle<state_value>>
{
    static const state value = state_value;
};

} // namespace detail

template <state state_value>
template <state other_value>
handle<state_value>::handle(handle<other_value>&& other)
    :
	basic_handle(static_cast<basic_handle&&>(std::move(other)))
{
    server_state = other_value;
}

template <typename func1_t, typename... funcn_t>
inline void with(basic_handle&& arg, func1_t&& head_func, funcn_t&&... tail_funcs)
{
    typedef detail::first_arg_type<func1_t> handle_arg_type;
    if (arg.server_state == detail::handle_traits<handle_arg_type>::value)
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
    if (arg.server_state == detail::handle_traits<handle_arg_type>::value)
    {
	func(static_cast<handle_arg_type&&>(std::move(arg)));
    }
}

} // namespace event
} // namespace server
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_SERVER_ENGINE_HXX
#define ROBOCUP2DSIM_SERVER_ENGINE_HXX

#include "engine.hpp"
#include <beam/message/capnproto.hxx>
#include <turbo/container/spsc_ring_queue.hxx>

namespace robocup2Dsim {
namespace server {
namespace engine {

template <state state_value>
handle<state_value>::handle(
	decltype(ref_input_queue) ref_in,
	decltype(ref_output_queue) ref_out,
	decltype(client_status_queue) client_status,
	decltype(client_trans_queue) client_trans,
	decltype(server_status_queue) server_status,
	decltype(server_trans_queue) server_trans)
    :
	basic_handle
	{
	    std::move(ref_input_queue),
	    std::move(ref_output_queue),
	    std::move(client_status_queue),
	    std::move(client_trans_queue),
	    std::move(server_status_queue),
	    std::move(server_trans_queue),
	    state_value
	}
{ }

template <state state_value>
template <state other_value>
handle<state_value>::handle(handle<other_value>&& other) :
	basic_handle
	{
	    std::move(other.ref_input_queue),
	    std::move(other.ref_output_queue),
	    std::move(other.client_status_queue),
	    std::move(other.client_trans_queue),
	    std::move(other.server_status_queue),
	    std::move(other.server_trans_queue),
	    other_value
	}
{ }

} // namespace engine
} // namespace server
} // namespace robocup2Dsim

#endif
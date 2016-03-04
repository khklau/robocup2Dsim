#ifndef ROBOCUP2DSIM_CLIENT_ENGINE_HXX
#define ROBOCUP2DSIM_CLIENT_ENGINE_HXX

#include "engine.hpp"
#include <beam/message/capnproto.hxx>
#include <turbo/container/spsc_ring_queue.hxx>

namespace robocup2Dsim {
namespace client {

template <state state_value>
handle<state_value>::handle(
	decltype(bot_input_queue) bot_in,
	decltype(bot_output_queue) bot_out,
	decltype(client_status_queue) client_status,
	decltype(client_trans_queue) client_trans,
	decltype(server_status_queue) server_status,
	decltype(server_trans_queue) server_trans)
    :
	basic_handle
	{
	    std::move(bot_in),
	    std::move(bot_out),
	    std::move(client_status),
	    std::move(client_trans),
	    std::move(server_status),
	    std::move(server_trans),
	    state_value
	}
{ }

} // namespace client
} // namespace robocup2Dsim

#endif

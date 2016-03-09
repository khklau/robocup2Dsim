#ifndef ROBOCUP2DSIM_CLIENT_ENGINE_HPP
#define ROBOCUP2DSIM_CLIENT_ENGINE_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <typeinfo>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace client {
namespace engine {

enum class state : uint8_t
{
    nobot_unregistered,
    nobot_onbench,
    withbot_unregistered,
    withbot_onbench,
    withbot_playing
};

struct basic_handle
{
public:
    std::unique_ptr<robocup2Dsim::bcprotocol::bot_input_queue_type> bot_input_queue;
    std::unique_ptr<robocup2Dsim::bcprotocol::bot_output_queue_type> bot_output_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::client_status_queue_type> client_status_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::client_trans_queue_type> client_trans_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::server_status_queue_type> server_status_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::server_trans_queue_type> server_trans_queue;
    state engine_state;
    basic_handle(
	    decltype(bot_input_queue) bot_in,
	    decltype(bot_output_queue) bot_out,
	    decltype(client_status_queue) client_status,
	    decltype(client_trans_queue) client_trans,
	    decltype(server_status_queue) server_status,
	    decltype(server_trans_queue) server_trans,
	    state my_state);
    basic_handle(basic_handle&& other);
    basic_handle& operator=(basic_handle&& other);
private:
    basic_handle() = delete;
    basic_handle(const basic_handle&) = delete;
    basic_handle& operator=(const basic_handle&) = delete;
};

template <state state_value>
struct handle : public basic_handle
{
public:
    handle(
	    decltype(bot_input_queue) bot_in,
	    decltype(bot_output_queue) bot_out,
	    decltype(client_status_queue) client_status,
	    decltype(client_trans_queue) client_trans,
	    decltype(server_status_queue) server_status,
	    decltype(server_trans_queue) server_trans);
    template <state other_state>
    handle(handle<other_state>&& other);
private:
    handle() = delete;
    handle(const handle<state_value>&) = delete;
    handle<state_value>& operator=(const handle<state_value>&) = delete;
};

template <state state_value>
inline basic_handle&& up_cast(handle<state_value>&& from)
{
    return static_cast<basic_handle&&>(from);
}

template <state state_value>
inline handle<state_value>&& down_cast(basic_handle&& from)
{
    if (from.engine_state == state_value)
    {
	return static_cast<handle<state_value>&&>(from);
    }
    else
    {
	throw std::bad_cast();
    }
}

handle<state::withbot_unregistered>&& spawned(handle<state::nobot_unregistered>&&);
handle<state::nobot_unregistered>&& bot_terminated(handle<state::withbot_unregistered>&&);
handle<state::withbot_unregistered>&& received_botoutput(handle<state::withbot_unregistered>&&, const robocup2Dsim::bcprotocol::BotOutput::Reader&);
handle<state::withbot_onbench>&& registered(handle<state::withbot_unregistered>&&);
handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_onbench>&&);
handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_onbench>&&);
handle<state::withbot_playing>&& field_opened(handle<state::withbot_onbench>&&);
handle<state::withbot_onbench>&& received_botoutput(handle<state::withbot_onbench>&&, const robocup2Dsim::bcprotocol::BotOutput::Reader&);
handle<state::nobot_onbench>&& bot_terminated(handle<state::withbot_onbench>&&);
handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_playing>&&);
handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_playing>&&);
handle<state::withbot_unregistered>&& match_over(handle<state::withbot_playing>&&);
handle<state::withbot_playing>&& simulation_timedout(handle<state::withbot_playing>&&);
handle<state::withbot_playing>&& sensor_timedout(handle<state::withbot_playing>&&);
handle<state::withbot_playing>&& status_timedout(handle<state::withbot_playing>&&);
handle<state::withbot_playing>&& received_botoutput(handle<state::withbot_playing>&&, const robocup2Dsim::bcprotocol::BotOutput::Reader&);
handle<state::withbot_playing>&& received_judgement(handle<state::withbot_playing>&&);
handle<state::withbot_playing>&& received_snapshot(handle<state::withbot_playing>&&);
handle<state::nobot_onbench>&& bot_terminated(handle<state::withbot_playing>&&);
handle<state::nobot_unregistered>&& disconnected(handle<state::nobot_onbench>&&);

} // namespace engine
} // namespace client
} // namespace robocup2Dsim

#endif

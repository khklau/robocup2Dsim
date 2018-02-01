#ifndef ROBOCUP2DSIM_CLIENT_EVENT_HPP
#define ROBOCUP2DSIM_CLIENT_EVENT_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <typeinfo>
#include <kj/array.h>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/toolset/extension.hpp>
#include "config.hpp"

namespace robocup2Dsim {
namespace client {
namespace event {

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
    kj::Array<capnp::word> bot_msg_buffer;
    kj::Array<capnp::word> server_msg_buffer;
    state client_state;
    basic_handle(
	    decltype(bot_input_queue) bot_in,
	    decltype(bot_output_queue) bot_out,
	    decltype(client_status_queue) client_status,
	    decltype(client_trans_queue) client_trans,
	    decltype(server_status_queue) server_status,
	    decltype(server_trans_queue) server_trans,
	    decltype(bot_msg_buffer) bot_msg,
	    decltype(server_msg_buffer) server_msg,
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
	    decltype(server_trans_queue) server_trans,
	    decltype(bot_msg_buffer) bot_msg,
	    decltype(server_msg_buffer) server_msg);
    template <state other_state>
    explicit handle(handle<other_state>&& other);
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
    if (TURBO_LIKELY(from.client_state == state_value))
    {
	return static_cast<handle<state_value>&&>(from);
    }
    else
    {
	throw std::bad_cast();
    }
}

handle<state::withbot_unregistered>&& spawned(handle<state::nobot_unregistered>&&, const robocup2Dsim::client::config& conf);

handle<state::withbot_onbench>&& registration_succeeded(handle<state::withbot_unregistered>&& input);
handle<state::withbot_unregistered>&& registration_failed(handle<state::withbot_unregistered>&& input, const robocup2Dsim::csprotocol::RegistrationError::Reader& reader);
handle<state::nobot_unregistered>&& bot_terminated(handle<state::withbot_unregistered>&& input);
handle<state::nobot_unregistered>&& bot_crashed(handle<state::withbot_unregistered>&& input);

handle<state::nobot_unregistered>&& disconnected(handle<state::nobot_onbench>&& input);

handle<state::withbot_playing>&& field_opened(handle<state::withbot_onbench>&& input, const robocup2Dsim::common::FieldOpen::Reader& reader);
handle<state::withbot_onbench>&& query_requested(handle<state::withbot_onbench>&& input, const robocup2Dsim::bcprotocol::QueryRequest::Reader& reader);
handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_onbench>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader);
handle<state::nobot_onbench>&& bot_crashed(handle<state::withbot_onbench>&& input);
handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_onbench>&& input);

handle<state::withbot_playing>&& received_snapshot(handle<state::withbot_playing>&&, const robocup2Dsim::csprotocol::ServerStatus::Reader& reader);
handle<state::withbot_playing>&& control_actioned(handle<state::withbot_playing>&&, const robocup2Dsim::common::PlayerAction::Reader& reader);
handle<state::withbot_playing>&& query_requested(handle<state::withbot_playing>&&, const robocup2Dsim::bcprotocol::QueryRequest::Reader& reader);
handle<state::withbot_playing>&& play_judged(handle<state::withbot_playing>&&, const robocup2Dsim::common::PlayJudgement::Reader& reader);
handle<state::withbot_playing>&& simulation_timedout(handle<state::withbot_playing>&& input);
handle<state::withbot_playing>&& sensor_timedout(handle<state::withbot_playing>&& input);
handle<state::withbot_playing>&& upload_timedout(handle<state::withbot_playing>&& input);
handle<state::withbot_unregistered>&& match_closed(handle<state::withbot_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader);
handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader);
handle<state::nobot_onbench>&& bot_crashed(handle<state::withbot_playing>&& input);
handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_playing>&& input);

} // namespace event
} // namespace client
} // namespace robocup2Dsim

#endif

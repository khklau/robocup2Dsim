#ifndef ROBOCUP2DSIM_SERVER_EVENT_HPP
#define ROBOCUP2DSIM_SERVER_EVENT_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <typeinfo>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <robocup2Dsim/srprotocol/protocol.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/toolset/extension.hpp>
#include "roster.hpp"

namespace robocup2Dsim {
namespace server {
namespace event {

enum class state : uint8_t
{
    noref_waiting,
    noref_playing,
    withref_waiting,
    withref_playing
};

struct basic_handle
{
public:
    std::unique_ptr<robocup2Dsim::srprotocol::ref_input_queue_type> ref_input_queue;
    std::unique_ptr<robocup2Dsim::srprotocol::ref_output_queue_type> ref_output_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::client_status_queue_type> client_status_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::client_trans_queue_type> client_trans_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::server_status_queue_type> server_status_queue;
    std::unique_ptr<robocup2Dsim::csprotocol::server_trans_queue_type> server_trans_queue;
    std::unique_ptr<robocup2Dsim::server::enrollment> enrollment;
    std::unique_ptr<robocup2Dsim::server::roster> roster;
    state event_state;
    basic_handle(
	    decltype(ref_input_queue) ref_in,
	    decltype(ref_output_queue) ref_out,
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
	    decltype(ref_input_queue) ref_in,
	    decltype(ref_output_queue) ref_out,
	    decltype(client_status_queue) client_status,
	    decltype(client_trans_queue) client_trans,
	    decltype(server_status_queue) server_status,
	    decltype(server_trans_queue) server_trans);
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
    if (TURBO_LIKELY(from.event_state == state_value))
    {
	return static_cast<handle<state_value>&&>(from);
    }
    else
    {
	throw std::bad_cast();
    }
}

handle<state::withref_waiting>&& ref_ready(handle<state::noref_waiting>&&);
handle<state::noref_waiting>&& registration_requested(handle<state::noref_waiting>&& input, const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);
handle<state::noref_waiting>&& disconnected(handle<state::noref_waiting>&& input);

handle<state::withref_playing>&& field_opened(handle<state::withref_waiting>&& input, const robocup2Dsim::common::FieldOpen::Reader& reader);
handle<state::withref_waiting>&& registration_requested(handle<state::withref_waiting>&& input, const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);
handle<state::withref_waiting>&& disconnected(handle<state::withref_waiting>&& input);
handle<state::noref_waiting>&& ref_crashed(handle<state::withref_waiting>&& input);

handle<state::withref_playing>&& status_uploaded(handle<state::withref_playing>&&, const robocup2Dsim::csprotocol::ClientStatus::Reader& reader);
handle<state::withref_playing>&& control_actioned(handle<state::withref_playing>&&, const robocup2Dsim::common::PlayerAction::Reader& reader);
handle<state::withref_playing>&& play_judged(handle<state::withref_playing>&&, const robocup2Dsim::common::PlayJudgement::Reader& reader);
handle<state::withref_playing>&& simulation_timedout(handle<state::withref_playing>&& input);
handle<state::withref_playing>&& snapshot_timedout(handle<state::withref_playing>&& input);
handle<state::withref_playing>&& registration_requested(handle<state::withref_playing>&& input, const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);
handle<state::withref_playing>&& disconnected(handle<state::withref_playing>&& input);
handle<state::noref_playing>&& ref_crashed(handle<state::withref_playing>&& input);
handle<state::withref_waiting>&& match_closed(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader);
handle<state::withref_waiting>&& match_aborted(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader);

handle<state::withref_playing>&& ref_ready(handle<state::noref_playing>&&);
handle<state::noref_playing>&& status_uploaded(handle<state::noref_playing>&&, const robocup2Dsim::csprotocol::ClientStatus::Reader& reader);
handle<state::noref_playing>&& control_actioned(handle<state::noref_playing>&&, const robocup2Dsim::common::PlayerAction::Reader& reader);
handle<state::noref_playing>&& simulation_timedout(handle<state::noref_playing>&& input);
handle<state::noref_playing>&& snapshot_timedout(handle<state::noref_playing>&& input);
handle<state::noref_playing>&& registration_requested(handle<state::noref_playing>&& input, const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);
handle<state::noref_playing>&& disconnected(handle<state::noref_playing>&& input);

} // namespace event
} // namespace server
} // namespace robocup2Dsim

#endif

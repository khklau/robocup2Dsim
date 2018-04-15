#ifndef ROBOCUP2DSIM_SERVER_EVENT_HPP
#define ROBOCUP2DSIM_SERVER_EVENT_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <typeinfo>
#include <beam/message/buffer_pool.hpp>
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
    robocup2Dsim::srprotocol::ref_input_queue_type::producer* ref_input_producer;
    robocup2Dsim::srprotocol::ref_output_queue_type::consumer* ref_output_consumer;
    robocup2Dsim::csprotocol::server_status_queue_type::producer* server_status_producer;
    robocup2Dsim::csprotocol::server_trans_queue_type::producer* server_trans_producer;
    robocup2Dsim::csprotocol::client_status_queue_type::consumer* client_status_consumer;
    robocup2Dsim::csprotocol::client_trans_queue_type::consumer* client_trans_consumer;
    std::unique_ptr<beam::message::buffer_pool> ref_inbound_buffer_pool;
    std::unique_ptr<beam::message::buffer_pool> ref_outbound_buffer_pool;
    std::unique_ptr<beam::message::buffer_pool> server_outbound_buffer_pool;
    std::unique_ptr<beam::message::buffer_pool> client_inbound_buffer_pool;
    std::unique_ptr<robocup2Dsim::server::enrollment> enrollment;
    std::unique_ptr<robocup2Dsim::server::roster> roster;
    state server_state;
    basic_handle(
	    decltype(ref_input_producer) ref_in,
	    decltype(ref_output_consumer) ref_out,
	    decltype(server_status_producer) server_status,
	    decltype(server_trans_producer) server_trans,
	    decltype(client_status_consumer) client_status,
	    decltype(client_trans_consumer) client_trans,
	    decltype(ref_inbound_buffer_pool)&& ref_inbound_pool,
	    decltype(ref_outbound_buffer_pool)&& ref_outbound_pool,
	    decltype(server_outbound_buffer_pool)&& server_outbound_pool,
	    decltype(client_inbound_buffer_pool)&& client_inbound_pool,
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
	    decltype(ref_input_producer) ref_in,
	    decltype(ref_output_consumer) ref_out,
	    decltype(server_status_producer) server_status,
	    decltype(server_trans_producer) server_trans,
	    decltype(client_status_consumer) client_status,
	    decltype(client_trans_consumer) client_trans,
	    decltype(ref_inbound_buffer_pool)&& ref_inbound_pool,
	    decltype(ref_outbound_buffer_pool)&& ref_outbound_pool,
	    decltype(server_outbound_buffer_pool)&& server_outbound_pool,
	    decltype(client_inbound_buffer_pool)&& client_inbound_pool);
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
    if (TURBO_LIKELY(from.server_state == state_value))
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

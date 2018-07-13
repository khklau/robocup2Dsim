#ifndef ROBOCUP2DSIM_SERVER_EVENT_HPP
#define ROBOCUP2DSIM_SERVER_EVENT_HPP

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>
#include <beam/internet/endpoint.hpp>
#include <beam/message/buffer_pool.hpp>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <robocup2Dsim/srprotocol/protocol.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/toolset/extension.hpp>
#include "config.hpp"
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

// TODO: implement the handle using std::variant when its available

template <state state_value>
struct handle;

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
    template <state state_value>
    basic_handle& operator=(handle<state_value>&& other);
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
    handle(handle<other_state>&& other);
    handle(basic_handle&& other);
private:
    handle() = delete;
    handle(const handle<state_value>&) = delete;
    handle<state_value>& operator=(const handle<state_value>&) = delete;
};

template <typename func1_t, typename... funcn_t>
inline void with(basic_handle&& arg, func1_t&& head_func, funcn_t&&... tail_funcs);

template <typename func_t>
inline void with(basic_handle&& arg, func_t&& func);


handle<state::withref_waiting> ref_spawned(
	handle<state::noref_waiting>&&,
	const robocup2Dsim::server::config& config);

handle<state::noref_waiting> registration_requested(
	handle<state::noref_waiting>&& input,
	beam::internet::endpoint_id source,
	const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);

handle<state::noref_waiting> disconnected(handle<state::noref_waiting>&& input);

handle<state::withref_playing> field_opened(handle<state::withref_waiting>&& input, const robocup2Dsim::common::FieldOpen::Reader& reader);

handle<state::withref_waiting> registration_requested(
	handle<state::withref_waiting>&& input,
	beam::internet::endpoint_id source,
	const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);

handle<state::withref_waiting> disconnected(handle<state::withref_waiting>&& input);
handle<state::noref_waiting> ref_crashed(handle<state::withref_waiting>&& input);

handle<state::withref_playing> status_uploaded(handle<state::withref_playing>&&, const robocup2Dsim::csprotocol::ClientStatus::Reader& reader);
handle<state::withref_playing> control_actioned(handle<state::withref_playing>&&, const robocup2Dsim::common::PlayerAction::Reader& reader);
handle<state::withref_playing> play_judged(handle<state::withref_playing>&&, const robocup2Dsim::common::PlayJudgement::Reader& reader);
handle<state::withref_playing> simulation_timedout(handle<state::withref_playing>&& input);
handle<state::withref_playing> snapshot_timedout(handle<state::withref_playing>&& input);

handle<state::withref_playing> registration_requested(
	handle<state::withref_playing>&& input,
	beam::internet::endpoint_id source,
	const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);

handle<state::withref_playing> disconnected(handle<state::withref_playing>&& input);
handle<state::noref_playing> ref_crashed(handle<state::withref_playing>&& input);
handle<state::withref_waiting> match_closed(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader);
handle<state::withref_waiting> match_aborted(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader);

handle<state::withref_playing> ref_spawned(handle<state::noref_playing>&&, const robocup2Dsim::server::config& config);
handle<state::noref_playing> status_uploaded(handle<state::noref_playing>&&, const robocup2Dsim::csprotocol::ClientStatus::Reader& reader);
handle<state::noref_playing> control_actioned(handle<state::noref_playing>&&, const robocup2Dsim::common::PlayerAction::Reader& reader);
handle<state::noref_playing> simulation_timedout(handle<state::noref_playing>&& input);
handle<state::noref_playing> snapshot_timedout(handle<state::noref_playing>&& input);

handle<state::noref_playing> registration_requested(
	handle<state::noref_playing>&& input,
	beam::internet::endpoint_id source,
	const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);

handle<state::noref_playing> disconnected(handle<state::noref_playing>&& input);

} // namespace event
} // namespace server
} // namespace robocup2Dsim

#endif

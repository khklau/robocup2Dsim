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
#include <robocup2Dsim/csprotocol/status.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/toolset/extension.hpp>
#include "config.hpp"
#include "monitor.hpp"
#include "roster.hpp"
#include "state.hpp"

namespace robocup2Dsim {
namespace server {
namespace event {

enum class state : uint8_t
{
    waiting,
    onbreak,
    playing
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
    std::unique_ptr<robocup2Dsim::server::monitor> monitor;
    std::unique_ptr<robocup2Dsim::server::server_game_state> game_state;
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


handle<state::waiting> ref_spawned(
	handle<state::waiting>&&,
	const robocup2Dsim::server::config& config);

handle<state::waiting> registration_requested(
	handle<state::waiting>&& input,
	beam::internet::endpoint_id source,
	const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);

handle<state::onbreak> roster_finalised(
        handle<state::waiting>&& input);

handle<state::waiting> disconnected(
        handle<state::waiting>&& input);

handle<state::waiting> ref_crashed(
        handle<state::waiting>&& input);


handle<state::onbreak> received_pong(
        handle<state::onbreak>&& input,
        beam::internet::endpoint_id client,
        const robocup2Dsim::csprotocol::Pong::Reader& pong);


handle<state::playing> field_opened(
        handle<state::onbreak>&& input,
        const robocup2Dsim::common::FieldOpen::Reader& reader);

handle<state::onbreak> ping_timedout(
        handle<state::onbreak>&& input);

handle<state::onbreak> ref_crashed(
        handle<state::onbreak>&& input);


handle<state::playing> status_uploaded(
        handle<state::playing>&&,
        const robocup2Dsim::csprotocol::ClientStatus::Reader& reader);

handle<state::playing> control_actioned(
        handle<state::playing>&&,
        const robocup2Dsim::common::PlayerAction::Reader& reader);

handle<state::playing> play_judged(
        handle<state::playing>&&,
        const robocup2Dsim::common::PlayJudgement::Reader& reader);

handle<state::playing> simulation_timedout(
        handle<state::playing>&& input);

handle<state::playing> snapshot_timedout(
        handle<state::playing>&& input);

handle<state::playing> registration_requested(
	handle<state::playing>&& input,
	beam::internet::endpoint_id source,
	const robocup2Dsim::csprotocol::RegistrationRequest::Reader& reader);

handle<state::playing> disconnected(
        handle<state::playing>&& input);

handle<state::playing> ref_crashed(
        handle<state::playing>&& input);

handle<state::waiting> match_closed(
        handle<state::playing>&& input,
        const robocup2Dsim::common::MatchClose::Reader& reader);

handle<state::waiting> match_aborted(
        handle<state::playing>&& input,
        const robocup2Dsim::common::MatchAbort::Reader& reader);

handle<state::playing> ref_spawned(
        handle<state::playing>&&,
        const robocup2Dsim::server::config& config);

} // namespace event
} // namespace server
} // namespace robocup2Dsim

#endif

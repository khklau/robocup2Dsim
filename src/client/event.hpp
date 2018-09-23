#ifndef ROBOCUP2DSIM_CLIENT_EVENT_HPP
#define ROBOCUP2DSIM_CLIENT_EVENT_HPP

#include "config.hpp"
#include "state.hpp"
#include <beam/message/buffer_pool.hpp>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/toolset/extension.hpp>
#include <kj/array.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <typeinfo>

namespace robocup2Dsim {
namespace client {
namespace event {

enum class state : uint8_t
{
    unregistered,
    onbench,
    playing
};

template <state state_value>
struct handle;

struct basic_handle
{
public:
    robocup2Dsim::bcprotocol::bot_input_queue_type::producer* bot_input_producer;
    robocup2Dsim::bcprotocol::bot_output_queue_type::consumer* bot_output_consumer;
    robocup2Dsim::csprotocol::client_status_queue_type::producer* client_status_producer;
    robocup2Dsim::csprotocol::client_trans_queue_type::producer* client_trans_producer;
    robocup2Dsim::csprotocol::server_status_queue_type::consumer* server_status_consumer;
    robocup2Dsim::csprotocol::server_trans_queue_type::consumer* server_trans_consumer;
    std::unique_ptr<beam::message::buffer_pool> bot_inbound_buffer_pool;
    std::unique_ptr<beam::message::buffer_pool> bot_outbound_buffer_pool;
    std::unique_ptr<beam::message::buffer_pool> client_outbound_buffer_pool;
    std::unique_ptr<beam::message::buffer_pool> server_inbound_buffer_pool;
    std::unique_ptr<client_game_state> game_state;
    state client_state;
    basic_handle(
	    decltype(bot_input_producer) bot_in,
	    decltype(bot_output_consumer) bot_out,
	    decltype(client_status_producer) client_status,
	    decltype(client_trans_producer) client_trans,
	    decltype(server_status_consumer) server_status,
	    decltype(server_trans_consumer) server_trans,
	    decltype(bot_inbound_buffer_pool)&& bot_inbound_pool,
	    decltype(bot_outbound_buffer_pool)&& bot_outbound_pool,
	    decltype(client_outbound_buffer_pool)&& client_outbound_pool,
	    decltype(server_inbound_buffer_pool)&& server_inbound_pool,
	    decltype(game_state)&& game,
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
	    decltype(bot_input_producer) bot_in,
	    decltype(bot_output_consumer) bot_out,
	    decltype(client_status_producer) client_status,
	    decltype(client_trans_producer) client_trans,
	    decltype(server_status_consumer) server_status,
	    decltype(server_trans_consumer) server_trans,
	    decltype(bot_inbound_buffer_pool)&& bot_inbound_pool,
	    decltype(bot_outbound_buffer_pool)&& bot_outbound_pool,
	    decltype(client_outbound_buffer_pool)&& client_outbound_pool,
	    decltype(server_inbound_buffer_pool)&& server_inbound_pool,
	    decltype(game_state)&& game);
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

handle<state::unregistered> spawned(
        handle<state::unregistered>&&,
        const robocup2Dsim::client::config& conf);

handle<state::onbench> registration_succeeded(
        handle<state::unregistered>&& input,
        const robocup2Dsim::csprotocol::RegistrationAck::Reader& reader);

handle<state::unregistered> registration_failed(
        handle<state::unregistered>&& input,
        const robocup2Dsim::csprotocol::RegistrationError::Reader& reader);

handle<state::unregistered> bot_terminated(
        handle<state::unregistered>&& input);

handle<state::unregistered> bot_crashed(
        handle<state::unregistered>&& input);


handle<state::playing> field_opened(
        handle<state::onbench>&& input,
        const robocup2Dsim::common::FieldOpen::Reader& reader);

handle<state::onbench> query_requested(
        handle<state::onbench>&& input,
        const robocup2Dsim::bcprotocol::QueryRequest::Reader& reader);

handle<state::unregistered> match_aborted(
        handle<state::onbench>&& input,
        const robocup2Dsim::common::MatchAbort::Reader& reader);

handle<state::onbench> bot_crashed(
        handle<state::onbench>&& input);

handle<state::unregistered> disconnected(
        handle<state::onbench>&& input);


handle<state::playing> received_snapshot(
        handle<state::playing>&&,
        const robocup2Dsim::csprotocol::ServerStatus::Reader& reader);

handle<state::playing> control_actioned(
        handle<state::playing>&&,
        const robocup2Dsim::common::PlayerAction::Reader& reader);

handle<state::playing> query_requested(
        handle<state::playing>&&,
        const robocup2Dsim::bcprotocol::QueryRequest::Reader& reader);

handle<state::playing> play_judged(
        handle<state::playing>&&,
        const robocup2Dsim::common::PlayJudgement::Reader& reader);

handle<state::playing> simulation_timedout(
        handle<state::playing>&& input);

handle<state::playing> sensor_timedout(
        handle<state::playing>&& input);

handle<state::playing> upload_timedout(
        handle<state::playing>&& input);

handle<state::unregistered> match_closed(
        handle<state::playing>&& input,
        const robocup2Dsim::common::MatchClose::Reader& reader);

handle<state::unregistered> match_aborted(
        handle<state::playing>&& input,
        const robocup2Dsim::common::MatchAbort::Reader& reader);

handle<state::onbench> bot_crashed(
        handle<state::playing>&& input);

handle<state::unregistered> disconnected(
        handle<state::playing>&& input);


} // namespace event
} // namespace client
} // namespace robocup2Dsim

#endif

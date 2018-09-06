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
    nobot_unregistered,
    nobot_onbench,
    withbot_unregistered,
    withbot_onbench,
    withbot_playing
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

handle<state::withbot_unregistered> spawned(handle<state::nobot_unregistered>&&, const robocup2Dsim::client::config& conf);

handle<state::withbot_onbench> registration_succeeded(
        handle<state::withbot_unregistered>&& input,
        const robocup2Dsim::csprotocol::RegistrationAck::Reader& reader);
handle<state::withbot_unregistered> registration_failed(
        handle<state::withbot_unregistered>&& input,
        const robocup2Dsim::csprotocol::RegistrationError::Reader& reader);

handle<state::nobot_unregistered> bot_terminated(handle<state::withbot_unregistered>&& input);
handle<state::nobot_unregistered> bot_crashed(handle<state::withbot_unregistered>&& input);

handle<state::nobot_unregistered> disconnected(handle<state::nobot_onbench>&& input);

handle<state::withbot_playing> field_opened(handle<state::withbot_onbench>&& input, const robocup2Dsim::common::FieldOpen::Reader& reader);
handle<state::withbot_onbench> query_requested(handle<state::withbot_onbench>&& input, const robocup2Dsim::bcprotocol::QueryRequest::Reader& reader);
handle<state::withbot_unregistered> match_aborted(handle<state::withbot_onbench>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader);
handle<state::nobot_onbench> bot_crashed(handle<state::withbot_onbench>&& input);
handle<state::withbot_unregistered> disconnected(handle<state::withbot_onbench>&& input);

handle<state::withbot_playing> received_snapshot(handle<state::withbot_playing>&&, const robocup2Dsim::csprotocol::ServerStatus::Reader& reader);
handle<state::withbot_playing> control_actioned(handle<state::withbot_playing>&&, const robocup2Dsim::common::PlayerAction::Reader& reader);
handle<state::withbot_playing> query_requested(handle<state::withbot_playing>&&, const robocup2Dsim::bcprotocol::QueryRequest::Reader& reader);
handle<state::withbot_playing> play_judged(handle<state::withbot_playing>&&, const robocup2Dsim::common::PlayJudgement::Reader& reader);
handle<state::withbot_playing> simulation_timedout(handle<state::withbot_playing>&& input);
handle<state::withbot_playing> sensor_timedout(handle<state::withbot_playing>&& input);
handle<state::withbot_playing> upload_timedout(handle<state::withbot_playing>&& input);
handle<state::withbot_unregistered> match_closed(handle<state::withbot_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader);
handle<state::withbot_unregistered> match_aborted(handle<state::withbot_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader);
handle<state::nobot_onbench> bot_crashed(handle<state::withbot_playing>&& input);
handle<state::withbot_unregistered> disconnected(handle<state::withbot_playing>&& input);

} // namespace event
} // namespace client
} // namespace robocup2Dsim

#endif

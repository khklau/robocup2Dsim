#ifndef ROBOCUP2DSIM_CLIENT_SPECIFICATION_HPP
#define ROBOCUP2DSIM_CLIENT_SPECIFICATION_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <turbo/container/spsc_ring_queue.hpp>

namespace robocup2Dsim {
namespace client {

typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::bcprotocol::ClientTransmission>>> bot_in_queue_type;
typedef turbo::container::spsc_ring_queue<std::unique_ptr<beam::message::capnproto<robocup2Dsim::bcprotocol::BotTransmission>>> bot_out_queue_type;

enum class state : uint8_t
{
    nobot_unregistered,
    nobot_onbench,
    withbot_unregistered,
    withbot_onbench,
    withbot_playing
};

template <state val>
struct state_type
{
    static const state value = val;
};

typedef state_type<state::nobot_unregistered> nobot_unregistered;
typedef state_type<state::nobot_unregistered> nobot_unregistered;
typedef state_type<state::nobot_onbench> nobot_onbench;
typedef state_type<state::withbot_unregistered> withbot_unregistered;
typedef state_type<state::withbot_onbench> withbot_onbench;
typedef state_type<state::withbot_playing> withbot_playing;

class client_specification
{
public:
    withbot_unregistered spawned(nobot_unregistered);
    nobot_unregistered bot_terminated(withbot_unregistered);
    withbot_unregistered received_control(withbot_unregistered);
    withbot_unregistered received_communication(withbot_unregistered);
    withbot_onbench registered(withbot_unregistered);
    withbot_unregistered disconnected(withbot_onbench);
    withbot_unregistered match_aborted(withbot_onbench);
    withbot_playing field_opened(withbot_onbench);
    withbot_onbench received_control(withbot_onbench);
    withbot_onbench received_communication(withbot_onbench);
    nobot_onbench bot_terminated(withbot_onbench);
    withbot_unregistered disconnected(withbot_playing);
    withbot_unregistered match_aborted(withbot_playing);
    withbot_unregistered match_over(withbot_playing);
    withbot_playing sensor_timedout(withbot_playing);
    withbot_playing heartbeat_timedout(withbot_playing);
    withbot_playing received_control(withbot_playing);
    withbot_playing received_communication(withbot_playing);
    withbot_playing received_judgement(withbot_playing);
    withbot_playing received_snapshot(withbot_playing);
    nobot_onbench bot_terminated(withbot_playing);
    nobot_unregistered disconnected(nobot_onbench);
};

} // namespace client
} // namespace robocup2Dsim

#endif

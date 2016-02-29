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

enum class state : uint8_t
{
    nobot_unregistered,
    nobot_onbench,
    withbot_unregistered,
    withbot_onbench,
    withbot_playing
};

template <state enum_value>
struct handle
{
    static const state value = enum_value;
};

template <class impl>
class client_specification
{
public:
    handle<state::withbot_unregistered> spawned(handle<state::nobot_unregistered>);
    handle<state::nobot_unregistered> bot_terminated(handle<state::withbot_unregistered>);
    handle<state::withbot_unregistered> received_control(handle<state::withbot_unregistered>);
    handle<state::withbot_unregistered> received_communication(handle<state::withbot_unregistered>);
    handle<state::withbot_onbench> registered(handle<state::withbot_unregistered>);
    handle<state::withbot_unregistered> disconnected(handle<state::withbot_onbench>);
    handle<state::withbot_unregistered> match_aborted(handle<state::withbot_onbench>);
    handle<state::withbot_playing> field_opened(handle<state::withbot_onbench>);
    handle<state::withbot_onbench> received_control(handle<state::withbot_onbench>);
    handle<state::withbot_onbench> received_communication(handle<state::withbot_onbench>);
    handle<state::nobot_onbench> bot_terminated(handle<state::withbot_onbench>);
    handle<state::withbot_unregistered> disconnected(handle<state::withbot_playing>);
    handle<state::withbot_unregistered> match_aborted(handle<state::withbot_playing>);
    handle<state::withbot_unregistered> match_over(handle<state::withbot_playing>);
    handle<state::withbot_playing> sensor_timedout(handle<state::withbot_playing>);
    handle<state::withbot_playing> status_timedout(handle<state::withbot_playing>);
    handle<state::withbot_playing> received_control(handle<state::withbot_playing>);
    handle<state::withbot_playing> received_communication(handle<state::withbot_playing>);
    handle<state::withbot_playing> received_judgement(handle<state::withbot_playing>);
    handle<state::withbot_playing> received_snapshot(handle<state::withbot_playing>);
    handle<state::nobot_onbench> bot_terminated(handle<state::withbot_playing>);
    handle<state::nobot_unregistered> disconnected(handle<state::nobot_onbench>);
};

} // namespace client
} // namespace robocup2Dsim

#endif

#include "engine.hpp"
#include "engine.hxx"

namespace robocup2Dsim {
namespace client {
namespace engine {

handle<state::withbot_unregistered>&& spawned(handle<state::nobot_unregistered>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_unregistered>&& bot_terminated(handle<state::withbot_unregistered>&& input)
{
    handle<state::nobot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& received_control(handle<state::withbot_unregistered>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& received_communication(handle<state::withbot_unregistered>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_onbench>&& registered(handle<state::withbot_unregistered>&& input)
{
    handle<state::withbot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_onbench>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_onbench>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& field_opened(handle<state::withbot_onbench>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_onbench>&& received_control(handle<state::withbot_onbench>&& input)
{
    handle<state::withbot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_onbench>&& received_communication(handle<state::withbot_onbench>&& input)
{
    handle<state::withbot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_onbench>&& bot_terminated(handle<state::withbot_onbench>&& input)
{
    handle<state::nobot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& match_over(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& simulation_timedout(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& sensor_timedout(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& status_timedout(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& received_control(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& received_communication(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& received_judgement(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& received_snapshot(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_onbench>&& bot_terminated(handle<state::withbot_playing>&& input)
{
    handle<state::nobot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_unregistered>&& disconnected(handle<state::nobot_onbench>&& input)
{
    handle<state::nobot_unregistered> output(std::move(input));
    return std::move(output);
}

} // namespace engine
} // namespace client
} // namespace robocup2Dsim

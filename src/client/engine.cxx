#include "engine.hpp"
#include "engine.hxx"

namespace rbc = robocup2Dsim::bcprotocol;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;

namespace robocup2Dsim {
namespace client {
namespace engine {

basic_handle::basic_handle(
	decltype(bot_input_queue) bot_in,
	decltype(bot_output_queue) bot_out,
	decltype(client_status_queue) client_status,
	decltype(client_trans_queue) client_trans,
	decltype(server_status_queue) server_status,
	decltype(server_trans_queue) server_trans,
	state my_state)
    :
	bot_input_queue(std::move(bot_in)),
	bot_output_queue(std::move(bot_out)),
	client_status_queue(std::move(client_status)),
	client_trans_queue(std::move(client_trans)),
	server_status_queue(std::move(server_status)),
	server_trans_queue(std::move(server_trans)),
	engine_state(my_state)
{ }


basic_handle::basic_handle(basic_handle&& other) :
	bot_input_queue(std::move(other.bot_input_queue)),
	bot_output_queue(std::move(other.bot_output_queue)),
	client_status_queue(std::move(other.client_status_queue)),
	client_trans_queue(std::move(other.client_trans_queue)),
	server_status_queue(std::move(other.server_status_queue)),
	server_trans_queue(std::move(other.server_trans_queue)),
	engine_state(other.engine_state)
{ }

basic_handle& basic_handle::operator=(basic_handle&& other)
{
    bot_input_queue = std::move(other.bot_input_queue);
    bot_output_queue = std::move(other.bot_output_queue);
    client_status_queue = std::move(other.client_status_queue);
    client_trans_queue = std::move(other.client_trans_queue);
    server_status_queue = std::move(other.server_status_queue);
    server_trans_queue = std::move(other.server_trans_queue);
    engine_state = other.engine_state;
    return *this;
}

handle<state::withbot_unregistered>&& spawned(handle<state::nobot_unregistered>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_onbench>&& registration_succeeded(handle<state::withbot_unregistered>&& input)
{
    handle<state::withbot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& registration_failed(handle<state::withbot_unregistered>&& input, const rcs::RegistrationError::Reader& reader)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_unregistered>&& bot_terminated(handle<state::withbot_unregistered>&& input)
{
    handle<state::nobot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_unregistered>&& bot_crashed(handle<state::withbot_unregistered>&& input)
{
    handle<state::nobot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_unregistered>&& disconnected(handle<state::nobot_onbench>&& input)
{
    handle<state::nobot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& field_opened(handle<state::withbot_onbench>&& input, const rco::FieldOpen::Reader& reader)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_onbench>&& query_requested(handle<state::withbot_onbench>&& input, const rbc::QueryRequest::Reader& reader)
{
    handle<state::withbot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_onbench>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_onbench>&& bot_crashed(handle<state::withbot_onbench>&& input)
{
    handle<state::nobot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_onbench>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& received_snapshot(handle<state::withbot_playing>&& input, const rcs::ServerStatus::Reader& reader)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& control_actioned(handle<state::withbot_playing>&& input, const rbc::Control::Reader& reader)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& query_requested(handle<state::withbot_playing>&& input, const rbc::QueryRequest::Reader& reader)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_playing>&& play_judged(handle<state::withbot_playing>&& input, const rco::PlayJudgement::Reader& reader)
{
    handle<state::withbot_playing> output(std::move(input));
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

handle<state::withbot_playing>&& upload_timedout(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& match_closed(handle<state::withbot_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& match_aborted(handle<state::withbot_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

handle<state::nobot_onbench>&& bot_crashed(handle<state::withbot_playing>&& input)
{
    handle<state::nobot_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withbot_unregistered>&& disconnected(handle<state::withbot_playing>&& input)
{
    handle<state::withbot_unregistered> output(std::move(input));
    return std::move(output);
}

} // namespace engine
} // namespace client
} // namespace robocup2Dsim

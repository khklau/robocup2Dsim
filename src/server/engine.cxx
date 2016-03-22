#include "engine.hpp"
#include "engine.hxx"

namespace rsr = robocup2Dsim::srprotocol;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;

namespace robocup2Dsim {
namespace server {
namespace engine {

basic_handle::basic_handle(
	decltype(ref_input_queue) ref_in,
	decltype(ref_output_queue) ref_out,
	decltype(client_status_queue) client_status,
	decltype(client_trans_queue) client_trans,
	decltype(server_status_queue) server_status,
	decltype(server_trans_queue) server_trans,
	state my_state)
    :
	ref_input_queue(std::move(ref_in)),
	ref_output_queue(std::move(ref_out)),
	client_status_queue(std::move(client_status)),
	client_trans_queue(std::move(client_trans)),
	server_status_queue(std::move(server_status)),
	server_trans_queue(std::move(server_trans)),
	engine_state(my_state)
{ }


basic_handle::basic_handle(basic_handle&& other) :
	ref_input_queue(std::move(other.ref_input_queue)),
	ref_output_queue(std::move(other.ref_output_queue)),
	client_status_queue(std::move(other.client_status_queue)),
	client_trans_queue(std::move(other.client_trans_queue)),
	server_status_queue(std::move(other.server_status_queue)),
	server_trans_queue(std::move(other.server_trans_queue)),
	engine_state(other.engine_state)
{ }

basic_handle& basic_handle::operator=(basic_handle&& other)
{
    ref_input_queue = std::move(other.ref_input_queue);
    ref_output_queue = std::move(other.ref_output_queue);
    server_status_queue = std::move(other.server_status_queue);
    server_trans_queue = std::move(other.server_trans_queue);
    client_status_queue = std::move(other.client_status_queue);
    client_trans_queue = std::move(other.client_trans_queue);
    engine_state = other.engine_state;
    return *this;
}

handle<state::withref_idle>&& spawned(handle<state::noref_idle>&& input)
{
    handle<state::withref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::withref_onbench>&& registration_succeeded(handle<state::withref_idle>&& input)
{
    handle<state::withref_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withref_idle>&& registration_failed(handle<state::withref_idle>&& input, const rcs::RegistrationError::Reader& reader)
{
    handle<state::withref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::noref_idle>&& ref_terminated(handle<state::withref_idle>&& input)
{
    handle<state::noref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::noref_idle>&& ref_crashed(handle<state::withref_idle>&& input)
{
    handle<state::noref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::noref_idle>&& disconnected(handle<state::noref_onbench>&& input)
{
    handle<state::noref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& field_opened(handle<state::withref_onbench>&& input, const rco::FieldOpen::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_idle>&& match_aborted(handle<state::withref_onbench>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader)
{
    handle<state::withref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::noref_onbench>&& ref_crashed(handle<state::withref_onbench>&& input)
{
    handle<state::noref_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withref_idle>&& disconnected(handle<state::withref_onbench>&& input)
{
    handle<state::withref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& received_snapshot(handle<state::withref_playing>&& input, const rcs::ClientStatus::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& play_judged(handle<state::withref_playing>&& input, const rco::PlayJudgement::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& simulation_timedout(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& sensor_timedout(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& upload_timedout(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_idle>&& match_closed(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader)
{
    handle<state::withref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::withref_idle>&& match_aborted(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader)
{
    handle<state::withref_idle> output(std::move(input));
    return std::move(output);
}

handle<state::noref_onbench>&& ref_crashed(handle<state::withref_playing>&& input)
{
    handle<state::noref_onbench> output(std::move(input));
    return std::move(output);
}

handle<state::withref_idle>&& disconnected(handle<state::withref_playing>&& input)
{
    handle<state::withref_idle> output(std::move(input));
    return std::move(output);
}

} // namespace engine
} // namespace server
} // namespace robocup2Dsim

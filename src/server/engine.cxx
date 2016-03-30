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

handle<state::withref_waiting>&& ref_ready(handle<state::noref_waiting>&& input)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::noref_waiting>&& registration_requested(handle<state::noref_waiting>&& input, const rcs::RegistrationRequest::Reader& reader)
{
    handle<state::noref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::noref_waiting>&& disconnected(handle<state::noref_waiting>&& input)
{
    handle<state::noref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& field_opened(handle<state::withref_waiting>&& input, const rco::FieldOpen::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_waiting>&& registration_requested(handle<state::withref_waiting>&& input, const rcs::RegistrationRequest::Reader& reader)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_waiting>&& disconnected(handle<state::withref_waiting>&& input)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::noref_waiting>&& ref_crashed(handle<state::withref_waiting>&& input)
{
    handle<state::noref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& received_status(handle<state::withref_playing>&& input, const rcs::ClientStatus::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& received_transaction(handle<state::withref_playing>&& input, const rcs::ClientTransaction::Reader& reader)
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

handle<state::withref_playing>&& snapshot_timedout(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& registration_requested(handle<state::withref_playing>&& input, const rcs::RegistrationRequest::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& disconnected(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing>&& ref_crashed(handle<state::withref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_waiting>&& match_closed(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_waiting>&& match_aborted(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing>&& ref_ready(handle<state::noref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing>&& received_status(handle<state::noref_playing>&& input, const rcs::ClientStatus::Reader& reader)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing>&& received_transaction(handle<state::noref_playing>&& input, const rcs::ClientTransaction::Reader& reader)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing>&& simulation_timedout(handle<state::noref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing>&& snapshot_timedout(handle<state::noref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing>&& registration_requested(handle<state::noref_playing>&& input, const rcs::RegistrationRequest::Reader& reader)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing>&& disconnected(handle<state::noref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

} // namespace engine
} // namespace server
} // namespace robocup2Dsim

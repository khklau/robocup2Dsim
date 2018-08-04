#include "event.hpp"
#include "event.hh"
#include <turbo/algorithm/recovery.hpp>
#include <turbo/algorithm/recovery.hh>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/rule.capnp.h>

namespace bin = beam::internet;
namespace bmc = beam::message::capnproto;
namespace rsr = robocup2Dsim::srprotocol;
namespace rce = robocup2Dsim::common::entity;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;
namespace tar = turbo::algorithm::recovery;

namespace robocup2Dsim {
namespace server {
namespace event {

basic_handle::basic_handle(
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
	state my_state)
    :
	ref_input_producer(ref_in),
	ref_output_consumer(ref_out),
	server_status_producer(server_status),
	server_trans_producer(server_trans),
	client_status_consumer(client_status),
	client_trans_consumer(client_trans),
	ref_inbound_buffer_pool(std::move(ref_inbound_pool)),
	ref_outbound_buffer_pool(std::move(ref_outbound_pool)),
	server_outbound_buffer_pool(std::move(server_outbound_pool)),
	client_inbound_buffer_pool(std::move(client_inbound_pool)),
	enrollment(new robocup2Dsim::server::enrollment()),
	roster(),
	server_state(my_state)
{ }


basic_handle::basic_handle(basic_handle&& other)
    :
	ref_input_producer(other.ref_input_producer),
	ref_output_consumer(other.ref_output_consumer),
	server_status_producer(other.server_status_producer),
	server_trans_producer(other.server_trans_producer),
	client_status_consumer(other.client_status_consumer),
	client_trans_consumer(other.client_trans_consumer),
	ref_inbound_buffer_pool(std::move(other.ref_inbound_buffer_pool)),
	ref_outbound_buffer_pool(std::move(other.ref_outbound_buffer_pool)),
	server_outbound_buffer_pool(std::move(other.server_outbound_buffer_pool)),
	client_inbound_buffer_pool(std::move(other.client_inbound_buffer_pool)),
	enrollment(std::move(other.enrollment)),
	roster(std::move(other.roster)),
	server_state(other.server_state)
{
    other.ref_input_producer = nullptr;
    other.ref_output_consumer = nullptr;
    other.server_status_producer = nullptr;
    other.server_trans_producer = nullptr;
    other.client_status_consumer = nullptr;
    other.client_trans_consumer = nullptr;
}

basic_handle& basic_handle::operator=(basic_handle&& other)
{
    ref_input_producer = other.ref_input_producer,
    ref_output_consumer = other.ref_output_consumer,
    server_status_producer = other.server_status_producer,
    server_trans_producer = other.server_trans_producer,
    client_status_consumer = other.client_status_consumer,
    client_trans_consumer = other.client_trans_consumer,
    ref_inbound_buffer_pool = std::move(other.ref_inbound_buffer_pool),
    ref_outbound_buffer_pool = std::move(other.ref_outbound_buffer_pool),
    server_outbound_buffer_pool = std::move(other.server_outbound_buffer_pool),
    client_inbound_buffer_pool = std::move(other.client_inbound_buffer_pool),
    enrollment = std::move(other.enrollment),
    roster = std::move(other.roster),
    server_state = other.server_state;

    other.ref_input_producer = nullptr;
    other.ref_output_consumer = nullptr;
    other.server_status_producer = nullptr;
    other.server_trans_producer = nullptr;
    other.client_status_consumer = nullptr;
    other.client_trans_consumer = nullptr;

    return *this;
}

template <state state_value>
handle<state_value>::handle(basic_handle&& other)
    :
	basic_handle(std::move(other))
{
    assert(server_state == state_value);
}

handle<state::withref_waiting> ref_spawned(handle<state::noref_waiting>&& input, const robocup2Dsim::server::config& config)
{
    bmc::form<rsr::RefInput> form(std::move(input.ref_outbound_buffer_pool->borrow()));
    rsr::RefInput::Builder ref_input = form.build();
    rco::MatchRules::Builder rules = ref_input.initRules();
    rules.setHalfDuration(config.match_half_length);
    bmc::payload<rsr::RefInput> payload(std::move(bmc::serialise(*(input.ref_outbound_buffer_pool), form)));
    tar::retry_with_random_backoff([&]()
    {
	if (input.ref_input_producer->try_enqueue_move(std::move(payload)) == rsr::ref_input_queue_type::producer::result::success)
	{
	    return tar::try_state::done;
	}
	else
	{
	    return tar::try_state::retry;
	}
    });
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::noref_waiting> registration_requested(
	handle<state::noref_waiting>&& input,
	bin::endpoint_id source,
	const rcs::RegistrationRequest::Reader& reader)
{
    return std::move(detail::registration_requested<state::noref_waiting>(std::move(input), source, reader));
}

handle<state::withref_onbreak> roster_finalised(handle<state::withref_waiting>&& input)
{
    input.roster = input.enrollment->finalise();
    input.enrollment.reset();
    for (auto iter = input.roster->cbegin(); iter != input.roster->cend(); ++iter)
    {
        bmc::form<rcs::ServerTransaction> ack_form(std::move(input.server_outbound_buffer_pool->borrow()), *iter);
        rcs::ServerTransaction::Builder server_trans = ack_form.build();
        rcs::RegistrationAck::Builder ack = server_trans.initRegAck();
        rce::PlayerUniform::Builder uniform = ack.initUniform();

        rce::UniformNumber number = rce::UniformNumber::ONE;
        rce::TeamId team = rce::TeamId::ALPHA;
        std::tie(number, team) = rce::id_to_uniform(iter.get_player_id());
        uniform.setUniform(number);
        uniform.setTeam(team);

        bmc::payload<rcs::ServerTransaction> payload(std::move(bmc::serialise(*(input.server_outbound_buffer_pool), ack_form)));
        tar::retry_with_random_backoff([&]()
        {
            if (input.server_trans_producer->try_enqueue_move(std::move(payload)) == rcs::server_trans_queue_type::producer::result::success)
            {
                return tar::try_state::done;
            }
            else
            {
                return tar::try_state::retry;
            }
        });
    }
    handle<state::withref_onbreak> output(std::move(input));
    return std::move(output);
}

handle<state::noref_waiting> disconnected(handle<state::noref_waiting>&& input)
{
    handle<state::noref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> field_opened(handle<state::withref_waiting>&& input, const rco::FieldOpen::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_waiting> registration_requested(
	handle<state::withref_waiting>&& input,
	bin::endpoint_id source,
	const rcs::RegistrationRequest::Reader& reader)
{
    return std::move(detail::registration_requested<state::withref_waiting>(std::move(input), source, reader));
}

handle<state::withref_waiting> disconnected(handle<state::withref_waiting>&& input)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::noref_waiting> ref_crashed(handle<state::withref_waiting>&& input)
{
    handle<state::noref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> status_uploaded(handle<state::withref_playing>&& input, const rcs::ClientStatus::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> control_actioned(handle<state::withref_playing>&& input, const rco::PlayerAction::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> play_judged(handle<state::withref_playing>&& input, const rco::PlayJudgement::Reader& reader)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> simulation_timedout(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> snapshot_timedout(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> registration_requested(
	handle<state::withref_playing>&& input,
	bin::endpoint_id source,
	const rcs::RegistrationRequest::Reader& reader)
{
    return std::move(detail::registration_requested<state::withref_playing>(std::move(input), source, reader));
}

handle<state::withref_playing> disconnected(handle<state::withref_playing>&& input)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing> ref_crashed(handle<state::withref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::withref_waiting> match_closed(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_waiting> match_aborted(handle<state::withref_playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader)
{
    handle<state::withref_waiting> output(std::move(input));
    return std::move(output);
}

handle<state::withref_playing> ref_spawned(handle<state::noref_playing>&& input, const robocup2Dsim::server::config& config)
{
    handle<state::withref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing> status_uploaded(handle<state::noref_playing>&& input, const rcs::ClientStatus::Reader& reader)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing> control_actioned(handle<state::noref_playing>&& input, const rco::PlayerAction::Reader& reader)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing> simulation_timedout(handle<state::noref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing> snapshot_timedout(handle<state::noref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

handle<state::noref_playing> registration_requested(
	handle<state::noref_playing>&& input,
	bin::endpoint_id source,
	const rcs::RegistrationRequest::Reader& reader)
{
    return std::move(detail::registration_requested<state::noref_playing>(std::move(input), source, reader));
}

handle<state::noref_playing> disconnected(handle<state::noref_playing>&& input)
{
    handle<state::noref_playing> output(std::move(input));
    return std::move(output);
}

} // namespace event
} // namespace server
} // namespace robocup2Dsim

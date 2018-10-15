#include "event.hpp"
#include "event.hh"
#include <turbo/algorithm/recovery.hpp>
#include <turbo/algorithm/recovery.hh>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/rule.capnp.h>
#include <robocup2Dsim/csprotocol/status.capnp.h>
#include <robocup2Dsim/runtime/db_access.hpp>

namespace bin = beam::internet;
namespace bmc = beam::message::capnproto;
namespace rsr = robocup2Dsim::srprotocol;
namespace rce = robocup2Dsim::common::entity;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;
namespace rru = robocup2Dsim::runtime;
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
	monitor(new robocup2Dsim::server::monitor()),
	game_state(),
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
	monitor(std::move(other.monitor)),
	game_state(std::move(other.game_state)),
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
    monitor = std::move(other.monitor),
    game_state = std::move(other.game_state),
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

handle<state::waiting> ref_spawned(handle<state::waiting>&& input, const robocup2Dsim::server::config& config)
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
    handle<state::waiting> output(std::move(input));
    return std::move(output);
}

handle<state::waiting> registration_requested(
	handle<state::waiting>&& input,
	bin::endpoint_id source,
	const rcs::RegistrationRequest::Reader& reader)
{
    return std::move(detail::registration_requested<state::waiting>(std::move(input), source, reader));
}

handle<state::onbreak> roster_finalised(handle<state::waiting>&& input)
{
    input.roster = input.enrollment->finalise();
    input.enrollment.reset();
    for (auto iter = input.roster->cbegin(); iter != input.roster->cend(); ++iter)
    {
        bin::endpoint_id client = *iter;
        bmc::form<rcs::ServerTransaction> ack_form(std::move(input.server_outbound_buffer_pool->borrow()), client);
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
    handle<state::onbreak> output(std::move(input));
    output.game_state.reset(new server_game_state(rru::update_local_db()));
    return std::move(output);
}

handle<state::waiting> disconnected(handle<state::waiting>&& input)
{
    handle<state::waiting> output(std::move(input));
    return std::move(output);
}

handle<state::waiting> ref_crashed(handle<state::waiting>&& input)
{
    handle<state::waiting> output(std::move(input));
    return std::move(output);
}

handle<state::playing> field_opened(handle<state::onbreak>&& input, const rco::FieldOpen::Reader& reader)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::onbreak> ping_timedout(handle<state::onbreak>&& input)
{
    std::uint16_t seq_num = input.monitor->clock.record_transmit();
    for (auto iter = input.roster->cbegin(); iter != input.roster->cend(); ++iter)
    {
        bin::endpoint_id client = *iter;
        bmc::form<rcs::ServerStatus> ping_form(std::move(input.server_outbound_buffer_pool->borrow()), client);
        rcs::ServerStatus::Builder server_status = ping_form.build();
        server_status.setPing();
        capnp::AnyPointer::Builder value1 = server_status.initValue1();
        rcs::Ping::Builder ping = value1.initAs<rcs::Ping>();
        ping.setSequence(seq_num);

        bmc::payload<rcs::ServerStatus> payload(std::move(bmc::serialise(*(input.server_outbound_buffer_pool), ping_form)));
        tar::retry_with_random_backoff([&]()
        {
            if (input.server_status_producer->try_enqueue_move(std::move(payload)) == rcs::server_status_queue_type::producer::result::success)
            {
                return tar::try_state::done;
            }
            else
            {
                return tar::try_state::retry;
            }
        });
    }
    handle<state::onbreak> output(std::move(input));
    return std::move(output);
}

handle<state::onbreak> ref_crashed(handle<state::onbreak>&& input)
{
    handle<state::waiting> output(std::move(input));
    return std::move(output);
}

handle<state::playing> status_uploaded(handle<state::playing>&& input, const rcs::ClientStatus::Reader& reader)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::playing> control_actioned(handle<state::playing>&& input, const rco::PlayerAction::Reader& reader)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::playing> play_judged(handle<state::playing>&& input, const rco::PlayJudgement::Reader& reader)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::playing> simulation_timedout(handle<state::playing>&& input)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::playing> snapshot_timedout(handle<state::playing>&& input)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::playing> registration_requested(
	handle<state::playing>&& input,
	bin::endpoint_id source,
	const rcs::RegistrationRequest::Reader& reader)
{
    return std::move(detail::registration_requested<state::playing>(std::move(input), source, reader));
}

handle<state::playing> disconnected(handle<state::playing>&& input)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::playing> ref_crashed(handle<state::playing>&& input)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

handle<state::waiting> match_closed(handle<state::playing>&& input, const robocup2Dsim::common::MatchClose::Reader& reader)
{
    handle<state::waiting> output(std::move(input));
    return std::move(output);
}

handle<state::waiting> match_aborted(handle<state::playing>&& input, const robocup2Dsim::common::MatchAbort::Reader& reader)
{
    handle<state::waiting> output(std::move(input));
    return std::move(output);
}

handle<state::playing> ref_spawned(handle<state::playing>&& input, const robocup2Dsim::server::config& config)
{
    handle<state::playing> output(std::move(input));
    return std::move(output);
}

} // namespace event
} // namespace server
} // namespace robocup2Dsim

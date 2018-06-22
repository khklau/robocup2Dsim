#include "event.hpp"
#include "event.hxx"
#include <robocup2Dsim/common/command.capnp.h>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/metadata.capnp.h>
#include <robocup2Dsim/csprotocol/command.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <turbo/algorithm/recovery.hpp>
#include <turbo/algorithm/recovery.hxx>

namespace bmc = beam::message::capnproto;
namespace rbc = robocup2Dsim::bcprotocol;
namespace rce = robocup2Dsim::common::entity;
namespace rcl = robocup2Dsim::client;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;
namespace tar = turbo::algorithm::recovery;

namespace robocup2Dsim {
namespace client {
namespace event {

basic_handle::basic_handle(
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
	state my_state)
    :
	bot_input_producer(bot_in),
	bot_output_consumer(bot_out),
	client_status_producer(client_status),
	client_trans_producer(client_trans),
	server_status_consumer(server_status),
	server_trans_consumer(server_trans),
	bot_inbound_buffer_pool(std::move(bot_inbound_pool)),
	bot_outbound_buffer_pool(std::move(bot_outbound_pool)),
	client_outbound_buffer_pool(std::move(client_outbound_pool)),
	server_inbound_buffer_pool(std::move(server_inbound_pool)),
	client_state(my_state)
{ }


basic_handle::basic_handle(basic_handle&& other) :
	bot_input_producer(other.bot_input_producer),
	bot_output_consumer(other.bot_output_consumer),
	client_status_producer(other.client_status_producer),
	client_trans_producer(other.client_trans_producer),
	server_status_consumer(other.server_status_consumer),
	server_trans_consumer(other.server_trans_consumer),
	bot_inbound_buffer_pool(std::move(other.bot_inbound_buffer_pool)),
	bot_outbound_buffer_pool(std::move(other.bot_outbound_buffer_pool)),
	client_outbound_buffer_pool(std::move(other.client_outbound_buffer_pool)),
	server_inbound_buffer_pool(std::move(other.server_inbound_buffer_pool)),
	client_state(other.client_state)
{
    other.bot_input_producer = nullptr;
    other.bot_output_consumer = nullptr;
    other.client_status_producer = nullptr;
    other.client_trans_producer = nullptr;
    other.server_status_consumer = nullptr;
    other.server_trans_consumer = nullptr;
}

basic_handle& basic_handle::operator=(basic_handle&& other)
{
    bot_input_producer = other.bot_input_producer;
    bot_output_consumer = other.bot_output_consumer;
    client_status_producer = other.client_status_producer;
    client_trans_producer = other.client_trans_producer;
    server_status_consumer = other.server_status_consumer;
    server_trans_consumer = other.server_trans_consumer;
    bot_inbound_buffer_pool = std::move(other.bot_inbound_buffer_pool);
    bot_outbound_buffer_pool = std::move(other.bot_outbound_buffer_pool);
    client_outbound_buffer_pool = std::move(other.client_outbound_buffer_pool);
    server_inbound_buffer_pool = std::move(other.server_inbound_buffer_pool);
    client_state = other.client_state;
    other.bot_input_producer = nullptr;
    other.bot_output_consumer = nullptr;
    other.client_status_producer = nullptr;
    other.client_trans_producer = nullptr;
    other.server_status_consumer = nullptr;
    other.server_trans_consumer = nullptr;
    return *this;
}

template <state state_value>
handle<state_value>::handle(basic_handle&& other)
    :
	basic_handle(std::move(other))
{
    assert(client_state == state_value);
}

handle<state::withbot_unregistered>&& spawned(handle<state::nobot_unregistered>&& input, const rcl::config& conf)
{
    bmc::form<rcs::ClientTransaction> form(std::move(input.client_outbound_buffer_pool->borrow()));
    rcs::ClientTransaction::Builder trans = form.build();
    rcs::RegistrationRequest::Builder request = trans.initRegistration();
    rco::command::Registration::Builder reg = request.initDetails();
    rco::metadata::Version::Builder version = reg.initVersion();
    version.setNumberA(1);
    version.setNumberB(0);
    version.setNumberC(0);
    version.setNumberD(0);
    reg.setTeamName(conf.team);
    reg.setUniform(conf.uniform);
    reg.setPlayerType(conf.goalie ? rce::PlayerType::GOAL_KEEPER : rce::PlayerType::OUT_FIELD);
    bmc::payload<rcs::ClientTransaction> payload(std::move(bmc::serialise(*(input.client_outbound_buffer_pool), form)));
    tar::retry_with_random_backoff([&]()
    {
	if (input.client_trans_producer->try_enqueue_move(std::move(payload)) == rcs::client_trans_queue_type::producer::result::success)
	{
	    return tar::try_state::done;
	}
	else
	{
	    return tar::try_state::retry;
	}
    });
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

handle<state::withbot_playing>&& control_actioned(handle<state::withbot_playing>&& input, const rco::PlayerAction::Reader& reader)
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

} // namespace event
} // namespace client
} // namespace robocup2Dsim

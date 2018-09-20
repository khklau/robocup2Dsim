#include <robocup2Dsim/server/event.hpp>
#include <robocup2Dsim/server/event.hh>
#include "test_utility.hpp"
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <robocup2Dsim/srprotocol/protocol.hpp>
#include <robocup2Dsim/common/metadata.hpp>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <turbo/type_utility/enum_iterator.hh>
#include <gtest/gtest.h>
#include <algorithm>
#include <unordered_set>
#include <utility>

namespace bin = beam::internet;
namespace bme = beam::message;
namespace bmc = beam::message::capnproto;
namespace rce = robocup2Dsim::common::entity;
namespace rco = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;
namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;
namespace rse = robocup2Dsim::server;
namespace rsr = robocup2Dsim::srprotocol;
namespace ttu = turbo::type_utility;

struct context
{
    const rse::config config_;
    rsr::ref_input_queue_type ref_input_queue_;
    rsr::ref_output_queue_type ref_output_queue_;
    rcs::server_status_queue_type server_status_queue_;
    rcs::server_trans_queue_type server_trans_queue_;
    rcs::client_status_queue_type client_status_queue_;
    rcs::client_trans_queue_type client_trans_queue_;
    rsr::ref_input_queue_type::producer& ref_input_producer_;
    rsr::ref_output_queue_type::consumer& ref_output_consumer_;
    rcs::server_status_queue_type::producer& server_status_producer_;
    rcs::server_trans_queue_type::producer& server_trans_producer_;
    rcs::client_status_queue_type::consumer& client_status_consumer_;
    rcs::client_trans_queue_type::consumer& client_trans_consumer_;
    rse::event::basic_handle handle_;

    context(const rse::config& config, rse::event::state state_value)
        :
            config_(config),
            ref_input_queue_(config_.ref_msg_queue_length),
            ref_output_queue_(config_.ref_msg_queue_length),
            server_status_queue_(config_.client_msg_queue_length),
            server_trans_queue_(config_.client_msg_queue_length),
            client_status_queue_(config_.client_msg_queue_length),
            client_trans_queue_(config_.client_msg_queue_length),
            ref_input_producer_(ref_input_queue_.get_producer()),
            ref_output_consumer_(ref_output_queue_.get_consumer()),
            server_status_producer_(server_status_queue_.get_producer()),
            server_trans_producer_(server_trans_queue_.get_producer()),
            client_status_consumer_(client_status_queue_.get_consumer()),
            client_trans_consumer_(client_trans_queue_.get_consumer()),
            handle_
            {
                &ref_input_producer_,
                &ref_output_consumer_,
                &server_status_producer_,
                &server_trans_producer_,
                &client_status_consumer_,
                &client_trans_consumer_,
                std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.ref_msg_word_length, config_.ref_msg_buffer_capacity))),
                std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.ref_msg_word_length, config_.ref_msg_buffer_capacity))),
                std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.client_msg_word_length, config_.client_msg_buffer_capacity))),
                std::move(std::unique_ptr<bme::buffer_pool>(new bme::buffer_pool(config_.client_msg_word_length, config_.client_msg_buffer_capacity))),
                state_value
            }
    { }
};

TEST(event_test_registration, roster_finalised_basic)
{
    rse::config config1;
    context context1(config1, rse::event::state::waiting);
    std::unordered_set<bin::endpoint_id> expected_clients1;

    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics* physics1_ptr = physics1.get();
    ren::register_system(rru::update_local_db(), std::move(physics1));

    bin::endpoint_id client(1000U, 12345U);
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUT_FIELD,
		*(context1.handle_.enrollment))) << "reg failed";
        expected_clients1.insert(client);
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUT_FIELD,
		*(context1.handle_.enrollment))) << "reg failed";
        expected_clients1.insert(client);
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    }
    ASSERT_TRUE(context1.handle_.enrollment->is_full());

    rse::event::with(std::move(context1.handle_),
        [&](rse::event::handle<rse::event::state::waiting>&& handle)
        {
            context1.handle_ = std::move(rse::event::roster_finalised(std::move(handle)));
        }
    );

    auto consumer = context1.server_trans_queue_.get_consumer();
    rcs::server_trans_queue_type::consumer::result dequeue_result = rcs::server_trans_queue_type::consumer::result::success;
    bmc::payload<rcs::ServerTransaction> payload;
    while (dequeue_result != rcs::server_trans_queue_type::consumer::result::queue_empty)
    {
        dequeue_result = consumer.try_dequeue_move(payload);
        if (dequeue_result == rcs::server_trans_queue_type::consumer::result::success)
        {
	    bmc::statement<rcs::ServerTransaction> trans(std::move(payload));
            ASSERT_TRUE(trans.read().isRegAck()) <<  "Roster finalisation did not produce RegistrationAck messages";
            rcs::RegistrationAck::Reader ack = trans.read().getRegAck();
            rce::PlayerUniform::Reader uniform = ack.getUniform();
            rce::player_id id = rce::uniform_to_id(uniform.getUniform(), uniform.getTeam());
            auto roster_iter = context1.handle_.roster->cbegin();
            for (; roster_iter != context1.handle_.roster->cend(); ++roster_iter)
            {
                if (roster_iter.get_player_id() == id)
                {
                    auto find_result = expected_clients1.find(*roster_iter);
                    ASSERT_NE(expected_clients1.cend(), find_result) << "RegistrationAck message produced for unknown player";
                    expected_clients1.erase(find_result);
                }
            }
        }
    }

    EXPECT_EQ(0U, expected_clients1.size()) << "RegistrationAck was not produced for all players";
}

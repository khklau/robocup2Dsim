#include <robocup2Dsim/server/roster.hpp>
#include <unordered_map>
#include <capnp/message.h>
#include <gtest/gtest.h>
#include <turbo/type_utility/enum_iterator.hpp>
#include <turbo/type_utility/enum_iterator.hxx>

namespace bin = beam::internet;
namespace rce = robocup2Dsim::common::entity;
namespace rcc = robocup2Dsim::common::command;
namespace rcm = robocup2Dsim::common::metadata;
namespace rcs = robocup2Dsim::csprotocol;
namespace rse = robocup2Dsim::server;
namespace ttu = turbo::type_utility;

rse::enrollment::register_result register_client(bin::endpoint_id client, const std::string& team, rce::UniformNumber uniform, rce::PlayerType ptype, rse::enrollment& enrollment)
{
    capnp::MallocMessageBuilder arena;
    rcs::RegistrationRequest::Builder request = arena.initRoot<rcs::RegistrationRequest>();
    rcc::Registration::Builder details = request.initDetails();
    rcm::Version::Builder version = details.initVersion();
    version.setNumberA(1U);
    version.setNumberB(0U);
    version.setNumberC(0U);
    version.setNumberD(0U);
    details.setTeamName(team);
    details.setUniform(uniform);
    details.setPlayerType(ptype);
    return enrollment.register_client(client, request.asReader());
}

TEST(enrollment_test, register_client_team_taken)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(1U, 12345U),
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(2U, 12345U),
	    "bar",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    EXPECT_EQ(rse::enrollment::register_result::team_slot_taken, register_client(
	    bin::endpoint_id(3U, 12345U),
	    "blah",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg succeeded";
}

TEST(enrollment_test, register_client_goalie_taken)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(1U, 12345U),
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(2U, 12345U),
	    "bar",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    EXPECT_EQ(rse::enrollment::register_result::goalie_taken, register_client(
	    bin::endpoint_id(3U, 12345U),
	    "foo",
	    rce::UniformNumber::TWO,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg succeeded";
    EXPECT_EQ(rse::enrollment::register_result::goalie_taken, register_client(
	    bin::endpoint_id(4U, 12345U),
	    "bar",
	    rce::UniformNumber::ELEVEN,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg succeeded";
}

TEST(enrollment_test, register_client_uniform_taken)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(1U, 12345U),
	    "foo",
	    rce::UniformNumber::FIVE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(2U, 12345U),
	    "bar",
	    rce::UniformNumber::FOUR,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    EXPECT_EQ(rse::enrollment::register_result::uniform_taken, register_client(
	    bin::endpoint_id(3U, 12345U),
	    "foo",
	    rce::UniformNumber::FIVE,
	    rce::PlayerType::OUT_FIELD,
	    enrollment1)) << "reg suceeded";
    EXPECT_EQ(rse::enrollment::register_result::uniform_taken, register_client(
	    bin::endpoint_id(4U, 12345U),
	    "bar",
	    rce::UniformNumber::FOUR,
	    rce::PlayerType::OUT_FIELD,
	    enrollment1)) << "reg suceeded";
}

TEST(enrollment_test, register_client_basic)
{
    rse::enrollment enrollment1;
    ASSERT_FALSE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should not be registered";
    ASSERT_FALSE(enrollment1.is_registered("foo", rce::UniformNumber::ONE)) << "should not be registered";
    ASSERT_FALSE(enrollment1.is_full());
    EXPECT_EQ(rse::enrollment::register_result::success, register_client(bin::endpoint_id(1U, 12345U), "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, enrollment1)) << "reg failed";
    EXPECT_TRUE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "is_registered return false on registered player";
    EXPECT_TRUE(enrollment1.is_registered("foo", rce::UniformNumber::ONE)) << "is_registered return false on registered player";
    EXPECT_FALSE(enrollment1.is_full());
}

TEST(enrollment_test, register_client_full)
{
    rse::enrollment enrollment1;
    bin::endpoint_id client(1000U, 12345U);
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
    }
    ASSERT_FALSE(enrollment1.is_full());
    client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "bar",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
    }
    EXPECT_TRUE(enrollment1.is_full());
    static const bin::endpoint_id end(1022U, 12345U);
    for (bin::endpoint_id client(1000U, 12345U); client < end; client = bin::endpoint_id(client.get_address() + 1U, client.get_port()))
    {
	EXPECT_TRUE(enrollment1.is_registered(client)) << "is_registered return false on registered player";
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	EXPECT_TRUE(enrollment1.is_registered("foo", uniform)) << "is_registered return false on registered player";
	EXPECT_TRUE(enrollment1.is_registered("bar", uniform)) << "is_registered return false on registered player";
    }
}

TEST(enrollment_test, deregister_client_invalid)
{
    rse::enrollment enrollment1;
    ASSERT_FALSE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should not be registered";
    EXPECT_EQ(rse::deregister_result::client_not_found, enrollment1.deregister_client(bin::endpoint_id(1U, 12345U)))
	    << "deregistered a non existing client";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(1U, 12345U),
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should be registered";
    ASSERT_EQ(rse::deregister_result::success, enrollment1.deregister_client(bin::endpoint_id(1U, 12345U)))
	    << "deregistered an existing client failed";
    ASSERT_FALSE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should be registered";
    EXPECT_EQ(rse::deregister_result::client_not_found, enrollment1.deregister_client(bin::endpoint_id(1U, 12345U)))
	    << "deregistering an already deregistered client succeeded";
}

TEST(enrollment_test, deregister_client_basic)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(1U, 12345U),
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should be registered";
    EXPECT_EQ(rse::deregister_result::success, enrollment1.deregister_client(bin::endpoint_id(1U, 12345U))) << "deregister failed";
    EXPECT_FALSE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should not be registered";
}

TEST(enrollment_test, register_and_deregister)
{
    rse::enrollment enrollment1;
    ASSERT_FALSE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should not be registered";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(1U, 12345U),
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should be registered";
    ASSERT_EQ(rse::deregister_result::success, enrollment1.deregister_client(bin::endpoint_id(1U, 12345U)))
	    << "deregistered an existing client failed";
    ASSERT_FALSE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should be registered";

    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    bin::endpoint_id(1U, 12345U),
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should be registered";
    EXPECT_EQ(rse::deregister_result::success, enrollment1.deregister_client(bin::endpoint_id(1U, 12345U)))
	    << "deregistered an existing client failed";
    EXPECT_FALSE(enrollment1.is_registered(bin::endpoint_id(1U, 12345U))) << "should be registered";
}

TEST(enrollment_test, finalise_invalid)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(bin::endpoint_id(1U, 12345U), "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, enrollment1)) << "reg failed";
    ASSERT_FALSE(enrollment1.is_full());
    EXPECT_FALSE(enrollment1.finalise()) << "roster was produced from a not full enrollment";
}

TEST(enrollment_test, finalise_almost_full)
{
    rse::enrollment enrollment1;
    bin::endpoint_id client(1U, 12345U);
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
    }
    ASSERT_FALSE(enrollment1.is_full());
    EXPECT_TRUE(enrollment1.finalise().get() == nullptr) << "roster was produced from a not full enrollment";
}

TEST(enrollment_test, finalise_basic)
{
    rse::enrollment enrollment1;
    bin::endpoint_id client(1000U, 12345U);
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    }
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    }
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "bar",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_full());
    auto roster1 = enrollment1.finalise();
    EXPECT_TRUE(roster1.get() != nullptr) << "finalise failed with full enrollment";
    EXPECT_STREQ("bar", roster1->get_team_name(rce::TeamId::ALPHA).c_str()) << "finalised teams were not sorted alphabetically";
    EXPECT_STREQ("foo", roster1->get_team_name(rce::TeamId::BETA).c_str()) << "finalised teams were not sorted alphabetically";
    std::unordered_map<bin::endpoint_id, rce::player_id> player_ids;
    static const bin::endpoint_id end(1022U, 12345U);
    for (client = bin::endpoint_id(1000U, 12345U); client < end; client = bin::endpoint_id(client.get_address() + 1U, client.get_port()))
    {
	auto result = roster1->find_player(client);
	EXPECT_EQ(rse::roster::find_result::found, std::get<0>(result)) << "find_player failed with client_id " << client;
	player_ids[client] = std::get<1>(result);
    }
    EXPECT_TRUE(roster1->is_goalkeeper(player_ids[bin::endpoint_id(1010U, 12345U)])) << "goalkeeper not finalised";
    EXPECT_TRUE(roster1->is_goalkeeper(player_ids[bin::endpoint_id(1021U, 12345U)])) << "goalkeeper not finalised";
    EXPECT_FALSE(roster1->is_goalkeeper(player_ids[bin::endpoint_id(1009U, 12345U)])) << "goalkeeper not finalised";
    EXPECT_FALSE(roster1->is_goalkeeper(player_ids[bin::endpoint_id(1020U, 12345U)])) << "goalkeeper not finalised";
    EXPECT_EQ(player_ids[bin::endpoint_id(1021U, 12345U)], 0) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1011U, 12345U)], 1) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1012U, 12345U)], 2) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1013U, 12345U)], 3) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1014U, 12345U)], 4) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1015U, 12345U)], 5) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1016U, 12345U)], 6) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1017U, 12345U)], 7) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1018U, 12345U)], 8) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1019U, 12345U)], 9) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1020U, 12345U)], 10) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1010U, 12345U)], 11) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1000U, 12345U)], 12) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1001U, 12345U)], 13) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1002U, 12345U)], 14) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1003U, 12345U)], 15) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1004U, 12345U)], 16) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1005U, 12345U)], 17) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1006U, 12345U)], 18) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1007U, 12345U)], 19) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1008U, 12345U)], 20) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[bin::endpoint_id(1009U, 12345U)], 21) << "finalise did not designated the correct player_id";
}

TEST(enrollment_test, finalise_no_goalkeeper)
{
    rse::enrollment enrollment1;
    bin::endpoint_id client(1000U, 12345U);
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUT_FIELD,
		enrollment1)) << "reg failed";
	client = bin::endpoint_id(client.get_address() + 1U, client.get_port());
    }
    ASSERT_TRUE(enrollment1.is_full());
    auto roster1 = enrollment1.finalise();
    EXPECT_TRUE(roster1.get() != nullptr) << "finalise failed with full enrollment";
    static const bin::endpoint_id end(1022U, 12345U);
    for (client = bin::endpoint_id(1000U, 12345U); client < end; client = bin::endpoint_id(client.get_address() + 1U, client.get_port()))
    {
	auto result = roster1->find_player(client);
	EXPECT_EQ(rse::roster::find_result::found, std::get<0>(result)) << "find_player failed with client_id " << client;
	EXPECT_FALSE(roster1->is_goalkeeper(std::get<1>(result))) << "goalkeeper finalised when none were registered for client" << client;
    }
}

TEST(roster_test, find_client_invalid)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(22U, 12345U), bin::endpoint_id(21U, 12345U), bin::endpoint_id(20U, 12345U),
	    bin::endpoint_id(19U, 12345U), bin::endpoint_id(18U, 12345U), bin::endpoint_id(17U, 12345U),
	    bin::endpoint_id(16U, 12345U), bin::endpoint_id(15U, 12345U), bin::endpoint_id(14U, 12345U),
	    bin::endpoint_id(13U, 12345U), bin::endpoint_id(12U, 12345U),
	    bin::endpoint_id(11U, 12345U), bin::endpoint_id(10U, 12345U), bin::endpoint_id(9U, 12345U),
	    bin::endpoint_id(8U, 12345U), bin::endpoint_id(7U, 12345U), bin::endpoint_id(6U, 12345U),
	    bin::endpoint_id(5U, 12345U), bin::endpoint_id(4U, 12345U), bin::endpoint_id(3U, 12345U),
	    bin::endpoint_id(2U, 12345U), bin::endpoint_id(1U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    EXPECT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_client(player_list1.max_size())))
	    << "find_client succeeded with an invalid player_id";
    EXPECT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_client(player_list1.max_size() + 1)))
	    << "find_client succeeded with an invalid player_id";
}

TEST(roster_test, find_client_basic)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(22U, 12345U), bin::endpoint_id(21U, 12345U), bin::endpoint_id(20U, 12345U),
	    bin::endpoint_id(19U, 12345U), bin::endpoint_id(18U, 12345U), bin::endpoint_id(17U, 12345U),
	    bin::endpoint_id(16U, 12345U), bin::endpoint_id(15U, 12345U), bin::endpoint_id(14U, 12345U),
	    bin::endpoint_id(13U, 12345U), bin::endpoint_id(12U, 12345U),
	    bin::endpoint_id(11U, 12345U), bin::endpoint_id(10U, 12345U), bin::endpoint_id(9U, 12345U),
	    bin::endpoint_id(8U, 12345U), bin::endpoint_id(7U, 12345U), bin::endpoint_id(6U, 12345U),
	    bin::endpoint_id(5U, 12345U), bin::endpoint_id(4U, 12345U), bin::endpoint_id(3U, 12345U),
	    bin::endpoint_id(2U, 12345U), bin::endpoint_id(1U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    for (std::size_t index = 0U; index < player_list1.max_size(); ++index)
    {
	auto result = roster1.find_client(index);
	EXPECT_EQ(rse::roster::find_result::found, std::get<0>(result)) << "find_client could not find the client_id";
	EXPECT_EQ(player_list1[index], std::get<1>(result)) << "find_client returned the wrong client_id";
    }
}

TEST(roster_test, find_player_invalid)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(22U, 12345U), bin::endpoint_id(21U, 12345U), bin::endpoint_id(20U, 12345U),
	    bin::endpoint_id(19U, 12345U), bin::endpoint_id(18U, 12345U), bin::endpoint_id(17U, 12345U),
	    bin::endpoint_id(16U, 12345U), bin::endpoint_id(15U, 12345U), bin::endpoint_id(14U, 12345U),
	    bin::endpoint_id(13U, 12345U), bin::endpoint_id(12U, 12345U),
	    bin::endpoint_id(11U, 12345U), bin::endpoint_id(10U, 12345U), bin::endpoint_id(9U, 12345U),
	    bin::endpoint_id(8U, 12345U), bin::endpoint_id(7U, 12345U), bin::endpoint_id(6U, 12345U),
	    bin::endpoint_id(5U, 12345U), bin::endpoint_id(4U, 12345U), bin::endpoint_id(3U, 12345U),
	    bin::endpoint_id(2U, 12345U), bin::endpoint_id(1U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    EXPECT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_player(bin::endpoint_id(0U, 12345U))))
	    << "find_player succeeded with an unknown client_id";
    EXPECT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_player(bin::endpoint_id(23U, 12345U))))
	    << "find_player succeeded with an unknown client_id";
}

TEST(roster_test, find_player_basic)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(1000U, 12345U), bin::endpoint_id(1001U, 12345U), bin::endpoint_id(1002U, 12345U),
	    bin::endpoint_id(1003U, 12345U), bin::endpoint_id(1004U, 12345U), bin::endpoint_id(1005U, 12345U),
	    bin::endpoint_id(1006U, 12345U), bin::endpoint_id(1007U, 12345U), bin::endpoint_id(1008U, 12345U),
	    bin::endpoint_id(1009U, 12345U), bin::endpoint_id(1010U, 12345U),
	    bin::endpoint_id(1011U, 12345U), bin::endpoint_id(1012U, 12345U), bin::endpoint_id(1013U, 12345U),
	    bin::endpoint_id(1014U, 12345U), bin::endpoint_id(1015U, 12345U), bin::endpoint_id(1016U, 12345U),
	    bin::endpoint_id(1017U, 12345U), bin::endpoint_id(1018U, 12345U), bin::endpoint_id(1019U, 12345U),
	    bin::endpoint_id(1020U, 12345U), bin::endpoint_id(1021U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    for (std::size_t index = 0U; index < player_list1.max_size(); ++index)
    {
	auto result = roster1.find_player(bin::endpoint_id(1000U + index, 12345U));
	EXPECT_EQ(rse::roster::find_result::found, std::get<0>(result)) << "find_player could not find the player_id";
	EXPECT_EQ(index, std::get<1>(result)) << "find_player returned the wrong player_id";
    }
}

TEST(roster_test, is_goalkeeper_invalid)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(22U, 12345U), bin::endpoint_id(21U, 12345U), bin::endpoint_id(20U, 12345U),
	    bin::endpoint_id(19U, 12345U), bin::endpoint_id(18U, 12345U), bin::endpoint_id(17U, 12345U),
	    bin::endpoint_id(16U, 12345U), bin::endpoint_id(15U, 12345U), bin::endpoint_id(14U, 12345U),
	    bin::endpoint_id(13U, 12345U), bin::endpoint_id(12U, 12345U),
	    bin::endpoint_id(11U, 12345U), bin::endpoint_id(10U, 12345U), bin::endpoint_id(9U, 12345U),
	    bin::endpoint_id(8U, 12345U), bin::endpoint_id(7U, 12345U), bin::endpoint_id(6U, 12345U),
	    bin::endpoint_id(5U, 12345U), bin::endpoint_id(4U, 12345U), bin::endpoint_id(3U, 12345U),
	    bin::endpoint_id(2U, 12345U), bin::endpoint_id(1U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    EXPECT_FALSE(roster1.is_goalkeeper(player_list1.max_size()))
	    << "is_goalkeeper succeeded with an invalid player_id";
    EXPECT_FALSE(roster1.is_goalkeeper(player_list1.max_size() + 1))
	    << "is_goalkeeper succeeded with an invalid player_id";
}

TEST(roster_test, is_goalkeeper_basic)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(22U, 12345U), bin::endpoint_id(21U, 12345U), bin::endpoint_id(20U, 12345U),
	    bin::endpoint_id(19U, 12345U), bin::endpoint_id(18U, 12345U), bin::endpoint_id(17U, 12345U),
	    bin::endpoint_id(16U, 12345U), bin::endpoint_id(15U, 12345U), bin::endpoint_id(14U, 12345U),
	    bin::endpoint_id(13U, 12345U), bin::endpoint_id(12U, 12345U),
	    bin::endpoint_id(11U, 12345U), bin::endpoint_id(10U, 12345U), bin::endpoint_id(9U, 12345U),
	    bin::endpoint_id(8U, 12345U), bin::endpoint_id(7U, 12345U), bin::endpoint_id(6U, 12345U),
	    bin::endpoint_id(5U, 12345U), bin::endpoint_id(4U, 12345U), bin::endpoint_id(3U, 12345U),
	    bin::endpoint_id(2U, 12345U), bin::endpoint_id(1U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    EXPECT_FALSE(roster1.is_goalkeeper(5))
	    << "is_goalkeeper returned true with player_id that isn't a goalkeeper";
    EXPECT_FALSE(roster1.is_goalkeeper(17))
	    << "is_goalkeeper returned true with player_id that isn't a goalkeeper";
    EXPECT_TRUE(roster1.is_goalkeeper(0))
	    << "is_goalkeeper returned false with player_id that is a goalkeeper";
    EXPECT_TRUE(roster1.is_goalkeeper(11))
	    << "is_goalkeeper returned false with player_id that is a goalkeeper";
}

TEST(roster_test, get_team_name_basic)
{
    const std::string ALPHA1("foo");
    const std::string BETA1("bar");
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1;
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{ALPHA1, BETA1};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1;
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    ASSERT_EQ(ALPHA1, roster1.get_team_name(rce::TeamId::ALPHA)) << "incorrect team name";
    ASSERT_EQ(BETA1, roster1.get_team_name(rce::TeamId::BETA)) << "incorrect team name";
}

TEST(roster_test, deregister_client_invalid)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(1000U, 12345U), bin::endpoint_id(1001U, 12345U), bin::endpoint_id(1002U, 12345U),
	    bin::endpoint_id(1003U, 12345U), bin::endpoint_id(1004U, 12345U), bin::endpoint_id(1005U, 12345U),
	    bin::endpoint_id(1006U, 12345U), bin::endpoint_id(1007U, 12345U), bin::endpoint_id(1008U, 12345U),
	    bin::endpoint_id(1009U, 12345U), bin::endpoint_id(1010U, 12345U),
	    bin::endpoint_id(1011U, 12345U), bin::endpoint_id(1012U, 12345U), bin::endpoint_id(1013U, 12345U),
	    bin::endpoint_id(1014U, 12345U), bin::endpoint_id(1015U, 12345U), bin::endpoint_id(1016U, 12345U),
	    bin::endpoint_id(1017U, 12345U), bin::endpoint_id(1018U, 12345U), bin::endpoint_id(1019U, 12345U),
	    bin::endpoint_id(1020U, 12345U), bin::endpoint_id(1021U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    ASSERT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_player(bin::endpoint_id(1U, 12345U)))) << "should not be registered";
    EXPECT_EQ(rse::deregister_result::client_not_found, roster1.deregister_client(bin::endpoint_id(1U, 12345U)))
	    << "deregistered a non existing client";
    ASSERT_EQ(rse::roster::find_result::found, std::get<0>(roster1.find_player(bin::endpoint_id(1010U, 12345U)))) << "should be registered";
    EXPECT_EQ(rse::deregister_result::success, roster1.deregister_client(bin::endpoint_id(1010U, 12345U)))
	    << "failed to deregistered an existing client";
    EXPECT_EQ(rse::deregister_result::client_not_found, roster1.deregister_client(bin::endpoint_id(1010U, 12345U)))
	    << "deregistered a client that had already been deregistered";
}

TEST(roster_test, deregister_client_basic)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(1000U, 12345U), bin::endpoint_id(1001U, 12345U), bin::endpoint_id(1002U, 12345U),
	    bin::endpoint_id(1003U, 12345U), bin::endpoint_id(1004U, 12345U), bin::endpoint_id(1005U, 12345U),
	    bin::endpoint_id(1006U, 12345U), bin::endpoint_id(1007U, 12345U), bin::endpoint_id(1008U, 12345U),
	    bin::endpoint_id(1009U, 12345U), bin::endpoint_id(1010U, 12345U),
	    bin::endpoint_id(1011U, 12345U), bin::endpoint_id(1012U, 12345U), bin::endpoint_id(1013U, 12345U),
	    bin::endpoint_id(1014U, 12345U), bin::endpoint_id(1015U, 12345U), bin::endpoint_id(1016U, 12345U),
	    bin::endpoint_id(1017U, 12345U), bin::endpoint_id(1018U, 12345U), bin::endpoint_id(1019U, 12345U),
	    bin::endpoint_id(1020U, 12345U), bin::endpoint_id(1021U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    static const bin::endpoint_id begin(1000U, 12345U);
    for (bin::endpoint_id client(1021U, 12345U); client >= begin; client = bin::endpoint_id(client.get_address() - 1U, client.get_port()))
    {
	ASSERT_EQ(rse::roster::find_result::found, std::get<0>(roster1.find_player(client))) << "should be registered";
	EXPECT_EQ(rse::deregister_result::success, roster1.deregister_client(client))
		<< "failed to deregistered an existing client";
	EXPECT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_player(client))) << "should not be registered";
    }
}

TEST(roster_test, deregister_client_goalkeeper)
{
    std::array<bin::endpoint_id, rse::MAX_ROSTER_SIZE> player_list1{
	    bin::endpoint_id(1000U, 12345U), bin::endpoint_id(1001U, 12345U), bin::endpoint_id(1002U, 12345U),
	    bin::endpoint_id(1003U, 12345U), bin::endpoint_id(1004U, 12345U), bin::endpoint_id(1005U, 12345U),
	    bin::endpoint_id(1006U, 12345U), bin::endpoint_id(1007U, 12345U), bin::endpoint_id(1008U, 12345U),
	    bin::endpoint_id(1009U, 12345U), bin::endpoint_id(1010U, 12345U),
	    bin::endpoint_id(1011U, 12345U), bin::endpoint_id(1012U, 12345U), bin::endpoint_id(1013U, 12345U),
	    bin::endpoint_id(1014U, 12345U), bin::endpoint_id(1015U, 12345U), bin::endpoint_id(1016U, 12345U),
	    bin::endpoint_id(1017U, 12345U), bin::endpoint_id(1018U, 12345U), bin::endpoint_id(1019U, 12345U),
	    bin::endpoint_id(1020U, 12345U), bin::endpoint_id(1021U, 12345U)};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    ASSERT_TRUE(roster1.is_goalkeeper(0)) << "should be a goalkeeper";
    EXPECT_EQ(rse::deregister_result::success, roster1.deregister_client(player_list1[0]))
	    << "failed to deregistered an existing client";
    ASSERT_FALSE(roster1.is_goalkeeper(0)) << "goalkeeper should be deregistered";
    ASSERT_TRUE(roster1.is_goalkeeper(11)) << "should be a goalkeeper";
    EXPECT_EQ(rse::deregister_result::success, roster1.deregister_client(player_list1[11]))
	    << "failed to deregistered an existing client";
    ASSERT_FALSE(roster1.is_goalkeeper(11)) << "goalkeeper should be deregistered";
    for (rce::player_id player = 0U; player < player_list1.max_size(); ++player)
    {
	ASSERT_FALSE(roster1.is_goalkeeper(player)) << "should not be a goalkeeper";
    }
}

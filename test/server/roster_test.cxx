#include <robocup2Dsim/server/roster.hpp>
#include <unordered_map>
#include <capnp/message.h>
#include <gtest/gtest.h>
#include <turbo/type_utility/enum_iterator.hpp>
#include <turbo/type_utility/enum_iterator.hxx>

namespace rce = robocup2Dsim::common::entity;
namespace rcc = robocup2Dsim::common::command;
namespace rcm = robocup2Dsim::common::metadata;
namespace rcs = robocup2Dsim::csprotocol;
namespace rse = robocup2Dsim::server;
namespace ttu = turbo::type_utility;

rse::enrollment::register_result register_client(rcs::client_id client, const std::string& team, rce::UniformNumber uniform, rce::PlayerType ptype, rse::enrollment& enrollment)
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
	    1U,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    2U,
	    "bar",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    EXPECT_EQ(rse::enrollment::register_result::team_slot_taken, register_client(
	    3U,
	    "blah",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg succeeded";
}

TEST(enrollment_test, register_client_goalie_taken)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    1U,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    2U,
	    "bar",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    EXPECT_EQ(rse::enrollment::register_result::goalie_taken, register_client(
	    3U,
	    "foo",
	    rce::UniformNumber::TWO,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg succeeded";
    EXPECT_EQ(rse::enrollment::register_result::goalie_taken, register_client(
	    4U,
	    "bar",
	    rce::UniformNumber::ELEVEN,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg succeeded";
}

TEST(enrollment_test, register_client_uniform_taken)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    1U,
	    "foo",
	    rce::UniformNumber::FIVE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    2U,
	    "bar",
	    rce::UniformNumber::FOUR,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    EXPECT_EQ(rse::enrollment::register_result::uniform_taken, register_client(
	    3U,
	    "foo",
	    rce::UniformNumber::FIVE,
	    rce::PlayerType::OUTFIELD,
	    enrollment1)) << "reg suceeded";
    EXPECT_EQ(rse::enrollment::register_result::uniform_taken, register_client(
	    4U,
	    "bar",
	    rce::UniformNumber::FOUR,
	    rce::PlayerType::OUTFIELD,
	    enrollment1)) << "reg suceeded";
}

TEST(enrollment_test, register_client_basic)
{
    rse::enrollment enrollment1;
    ASSERT_FALSE(enrollment1.is_registered(1U)) << "should not be registered";
    ASSERT_FALSE(enrollment1.is_registered("foo", rce::UniformNumber::ONE)) << "should not be registered";
    ASSERT_FALSE(enrollment1.is_full());
    EXPECT_EQ(rse::enrollment::register_result::success, register_client(1U, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, enrollment1)) << "reg failed";
    EXPECT_TRUE(enrollment1.is_registered(1U)) << "is_registered return false on registered player";
    EXPECT_TRUE(enrollment1.is_registered("foo", rce::UniformNumber::ONE)) << "is_registered return false on registered player";
    EXPECT_FALSE(enrollment1.is_full());
}

TEST(enrollment_test, register_client_full)
{
    rse::enrollment enrollment1;
    rcs::client_id client = 1000U;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
    }
    ASSERT_FALSE(enrollment1.is_full());
    ++client;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "bar",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
    }
    EXPECT_TRUE(enrollment1.is_full());
    for (rcs::client_id client = 1000U; client < 1022U; ++client)
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
    ASSERT_FALSE(enrollment1.is_registered(1U)) << "should not be registered";
    EXPECT_EQ(rse::enrollment::deregister_result::client_not_found, enrollment1.deregister_client(1U))
	    << "deregistered a non existing client";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    1U,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(1U)) << "should be registered";
    ASSERT_EQ(rse::enrollment::deregister_result::success, enrollment1.deregister_client(1U))
	    << "deregistered an existing client failed";
    ASSERT_FALSE(enrollment1.is_registered(1U)) << "should be registered";
    EXPECT_EQ(rse::enrollment::deregister_result::client_not_found, enrollment1.deregister_client(1U))
	    << "deregistering an already deregistered client succeeded";
}

TEST(enrollment_test, deregister_client_basic)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    1U,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(1U)) << "should be registered";
    EXPECT_EQ(rse::enrollment::deregister_result::success, enrollment1.deregister_client(1U)) << "deregister failed";
    EXPECT_FALSE(enrollment1.is_registered(1U)) << "should not be registered";
}

TEST(enrollment_test, register_and_deregister)
{
    rse::enrollment enrollment1;
    ASSERT_FALSE(enrollment1.is_registered(1U)) << "should not be registered";
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    1U,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(1U)) << "should be registered";
    ASSERT_EQ(rse::enrollment::deregister_result::success, enrollment1.deregister_client(1U))
	    << "deregistered an existing client failed";
    ASSERT_FALSE(enrollment1.is_registered(1U)) << "should be registered";

    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    1U,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ASSERT_TRUE(enrollment1.is_registered(1U)) << "should be registered";
    EXPECT_EQ(rse::enrollment::deregister_result::success, enrollment1.deregister_client(1U))
	    << "deregistered an existing client failed";
    EXPECT_FALSE(enrollment1.is_registered(1U)) << "should be registered";
}

TEST(enrollment_test, finalise_invalid)
{
    rse::enrollment enrollment1;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(1U, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, enrollment1)) << "reg failed";
    ASSERT_FALSE(enrollment1.is_full());
    EXPECT_FALSE(enrollment1.finalise()) << "roster was produced from a not full enrollment";
}

TEST(enrollment_test, finalise_almost_full)
{
    rse::enrollment enrollment1;
    rcs::client_id client = 1U;
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
    }
    ASSERT_FALSE(enrollment1.is_full());
    EXPECT_TRUE(enrollment1.finalise().get() == nullptr) << "roster was produced from a not full enrollment";
}

TEST(enrollment_test, finalise_basic)
{
    rse::enrollment enrollment1;
    rcs::client_id client = 1000U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
	++client;
    }
    ASSERT_EQ(rse::enrollment::register_result::success, register_client(
	    client,
	    "foo",
	    rce::UniformNumber::ONE,
	    rce::PlayerType::GOAL_KEEPER,
	    enrollment1)) << "reg failed";
    ++client;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
	++client;
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
    std::unordered_map<rcs::client_id, rce::player_id> player_ids;
    for (client = 1000U; client < 1022U; ++client)
    {
	auto result = roster1->find_player(client);
	EXPECT_EQ(rse::roster::find_result::found, std::get<0>(result)) << "find_player failed with client_id " << client;
	player_ids[client] = std::get<1>(result);
    }
    EXPECT_TRUE(roster1->is_goalkeeper(player_ids[1010])) << "goalkeeper not finalised";
    EXPECT_TRUE(roster1->is_goalkeeper(player_ids[1021])) << "goalkeeper not finalised";
    EXPECT_FALSE(roster1->is_goalkeeper(player_ids[1009])) << "goalkeeper not finalised";
    EXPECT_FALSE(roster1->is_goalkeeper(player_ids[1020])) << "goalkeeper not finalised";
    EXPECT_EQ(player_ids[1021], 0) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1011], 1) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1012], 2) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1013], 3) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1014], 4) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1015], 5) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1016], 6) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1017], 7) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1018], 8) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1019], 9) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1020], 10) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1010], 11) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1000], 12) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1001], 13) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1002], 14) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1003], 15) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1004], 16) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1005], 17) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1006], 18) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1007], 19) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1008], 20) << "finalise did not designated the correct player_id";
    EXPECT_EQ(player_ids[1009], 21) << "finalise did not designated the correct player_id";
}

TEST(enrollment_test, finalise_no_goalkeeper)
{
    rse::enrollment enrollment1;
    rcs::client_id client = 1000U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"foo",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
	++client;
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::enrollment::register_result::success, register_client(
		client,
		"bar",
		uniform,
		rce::PlayerType::OUTFIELD,
		enrollment1)) << "reg failed";
	++client;
    }
    ASSERT_TRUE(enrollment1.is_full());
    auto roster1 = enrollment1.finalise();
    EXPECT_TRUE(roster1.get() != nullptr) << "finalise failed with full enrollment";
    for (client = 1000U; client < 1022U; ++client)
    {
	auto result = roster1->find_player(client);
	EXPECT_EQ(rse::roster::find_result::found, std::get<0>(result)) << "find_player failed with client_id " << client;
	EXPECT_FALSE(roster1->is_goalkeeper(std::get<1>(result))) << "goalkeeper finalised when none were registered for client" << client;
    }
}

TEST(roster_test, find_client_invalid)
{
    std::array<rcs::client_id, rse::MAX_ROSTER_SIZE> player_list1{
	    22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
	    11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
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
    std::array<rcs::client_id, rse::MAX_ROSTER_SIZE> player_list1{
	    22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
	    11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
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
    std::array<rcs::client_id, rse::MAX_ROSTER_SIZE> player_list1{
	    22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
	    11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    EXPECT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_player(0U)))
	    << "find_player succeeded with an unknown client_id";
    EXPECT_EQ(rse::roster::find_result::not_found, std::get<0>(roster1.find_player(23)))
	    << "find_player succeeded with an unknown client_id";
}

TEST(roster_test, find_player_basic)
{
    std::array<rcs::client_id, rse::MAX_ROSTER_SIZE> player_list1{
	    1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010,
	    1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021};
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{"foo", "bar"};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1{0, 11};
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    for (std::size_t index = 0U; index < player_list1.max_size(); ++index)
    {
	auto result = roster1.find_player(1000 + index);
	EXPECT_EQ(rse::roster::find_result::found, std::get<0>(result)) << "find_player could not find the player_id";
	EXPECT_EQ(index, std::get<1>(result)) << "find_player returned the wrong player_id";
    }
}

TEST(roster_test, is_goalkeeper_invalid)
{
    std::array<rcs::client_id, rse::MAX_ROSTER_SIZE> player_list1{
	    22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
	    11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
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
    std::array<rcs::client_id, rse::MAX_ROSTER_SIZE> player_list1{
	    22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
	    11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
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
    std::array<rcs::client_id, rse::MAX_ROSTER_SIZE> player_list1;
    std::array<std::string, rse::MAX_CLUB_COUNT> team_list1{ALPHA1, BETA1};
    std::array<rce::player_id, rse::MAX_CLUB_COUNT> goalie_list1;
    rse::roster roster1(player_list1, team_list1, goalie_list1);
    ASSERT_EQ(ALPHA1, roster1.get_team_name(rce::TeamId::ALPHA)) << "incorrect team name";
    ASSERT_EQ(BETA1, roster1.get_team_name(rce::TeamId::BETA)) << "incorrect team name";
}

/*
TEST(roster_test, deregistration_basic)
{
    rse::roster roster1;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(1U, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    roster1.deregister_client(1U);
    ASSERT_EQ(rse::roster::registration_result::success, register_client(1U, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
}

TEST(roster_test, deregistration_complete)
{
    rse::roster roster1;
    rcs::client_id client = 1U;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "foo", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
    }
    ASSERT_FALSE(roster1.is_complete());
    ++client;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bar", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bar", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
    }
    ASSERT_TRUE(roster1.is_complete());
    roster1.deregister_client(8U);
    ASSERT_FALSE(roster1.is_complete());
}

TEST(roster_test, finalisation_complete)
{
    rse::roster roster1;
    ASSERT_FALSE(roster1.is_finalised());
    rcs::client_id client = 1U;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "foo", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
    }
    ++client;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bar", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bar", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster1.finalise()) << "finalisation failed";
    ASSERT_TRUE(roster1.is_finalised());
}

TEST(roster_test, finalisation_incomplete)
{
    rse::roster roster1;
    ASSERT_FALSE(roster1.is_finalised());
    rcs::client_id client1 = 1U;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client1, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client1;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client1, "foo", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
    }
    ++client1;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client1, "bar", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::THREE, rce::UniformNumber::ELEVEN>())
    {
	++client1;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client1, "bar", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
    }
    ASSERT_FALSE(roster1.is_complete());
    ASSERT_EQ(rse::roster::finalisation_result::roster_incomplete, roster1.finalise()) << "finalisation succeeded";
    ASSERT_FALSE(roster1.is_finalised());

    rse::roster roster2;
    ASSERT_FALSE(roster2.is_finalised());
    rcs::client_id client2 = 1U;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client2, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster2)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client2;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client2, "foo", uniform, rce::PlayerType::OUTFIELD, roster2)) << "reg failed";
    }
    ++client2;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client2, "bar", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster2)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client2;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client2, "bar", uniform, rce::PlayerType::OUTFIELD, roster2)) << "reg failed";
    }
    roster2.deregister_client(5U);
    ASSERT_FALSE(roster2.is_complete());
    ASSERT_EQ(rse::roster::finalisation_result::roster_incomplete, roster2.finalise()) << "finalisation succeeded";
    ASSERT_FALSE(roster2.is_finalised());
}
*/

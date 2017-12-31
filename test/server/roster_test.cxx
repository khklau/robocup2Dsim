#include <robocup2Dsim/server/roster.hpp>
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

rse::roster::registration_result register_client(rcs::client_id client, const std::string& team, rce::UniformNumber uniform, rce::PlayerType ptype, rse::roster& roster)
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
    return roster.register_client(client, request.asReader());
}

TEST(roster_test, registration_basic)
{
    rse::roster roster1;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(1U, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    ASSERT_FALSE(roster1.is_complete());
}

TEST(roster_test, registration_complete)
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
}

TEST(roster_test, registration_team_taken)
{
    rse::roster roster1;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(1U, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    ASSERT_EQ(rse::roster::registration_result::success, register_client(1U, "bar", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    ASSERT_EQ(rse::roster::registration_result::team_slot_taken, register_client(1U, "blah", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg succeeded";
}

TEST(roster_test, registration_uniform_taken)
{
    rse::roster roster1;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(1U, "foo", rce::UniformNumber::FIVE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    ASSERT_EQ(rse::roster::registration_result::success, register_client(2U, "bar", rce::UniformNumber::FOUR, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    ASSERT_EQ(rse::roster::registration_result::uniform_taken, register_client(3U, "foo", rce::UniformNumber::FIVE, rce::PlayerType::OUTFIELD, roster1)) << "reg suceeded";
    ASSERT_EQ(rse::roster::registration_result::uniform_taken, register_client(4U, "bar", rce::UniformNumber::FOUR, rce::PlayerType::OUTFIELD, roster1)) << "reg suceeded";
}

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

TEST(roster_test, get_team_name)
{
    rse::roster roster1;
    const char* ALPHA1 = "aaa";
    const char* BETA1 = "bbb";
    rcs::client_id client1 = 1U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client1, ALPHA1, uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client1;
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client1, BETA1, uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client1;
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster1.finalise()) << "finalisation failed";
    ASSERT_EQ(ALPHA1, roster1.get_team_name(rce::TeamId::ALPHA)) << "incorrect team name";
    ASSERT_EQ(BETA1, roster1.get_team_name(rce::TeamId::BETA)) << "incorrect team name";

    rse::roster roster2;
    const char* ALPHA2 = "AAAA";
    const char* BETA2 = "BBBB";
    rcs::client_id client2 = 1U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client2, BETA2, uniform, rce::PlayerType::OUTFIELD, roster2)) << "reg failed";
	++client2;
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client2, ALPHA2, uniform, rce::PlayerType::OUTFIELD, roster2)) << "reg failed";
	++client2;
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster2.finalise()) << "finalisation failed";
    ASSERT_EQ(ALPHA2, roster2.get_team_name(rce::TeamId::ALPHA)) << "incorrect team name";
    ASSERT_EQ(BETA2, roster2.get_team_name(rce::TeamId::BETA)) << "incorrect team name";
}

TEST(roster_test, is_registered_player)
{
    rse::roster roster1;
    ASSERT_FALSE(roster1.is_finalised());
    rcs::client_id client = 1U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "aaa", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bbb", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster1.finalise()) << "finalisation failed";

    for (auto team: ttu::enum_iterator<rce::TeamId, rce::TeamId::ALPHA, rce::TeamId::BETA>())
    {
	for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
	{
	    rce::old_player_id id{uniform, team};
	    ASSERT_TRUE(roster1.is_registered(id)) << "player is not registered";
	}
    }

    rce::old_player_id player1{rce::UniformNumber::TWO, rce::TeamId::ALPHA};
    roster1.deregister_client(2U);
    ASSERT_FALSE(roster1.is_registered(player1)) << "player is still registered";

    rce::old_player_id player2{rce::UniformNumber::TWO, rce::TeamId::BETA};
    roster1.deregister_client(13U);
    ASSERT_FALSE(roster1.is_registered(player2)) << "player is still registered";

    rce::old_player_id player3{rce::UniformNumber::ELEVEN, rce::TeamId::ALPHA};
    roster1.deregister_client(11U);
    ASSERT_FALSE(roster1.is_registered(player3)) << "player is still registered";

    rce::old_player_id player4{rce::UniformNumber::ELEVEN, rce::TeamId::BETA};
    roster1.deregister_client(22U);
    ASSERT_FALSE(roster1.is_registered(player4)) << "player is still registered";
}

TEST(roster_test, is_registered_client)
{
    rse::roster roster1;
    ASSERT_FALSE(roster1.is_finalised());
    rcs::client_id client = 1U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "aaa", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bbb", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster1.finalise()) << "finalisation failed";

    for (rcs::client_id client = 1U; client <= 22U; ++client)
    {
	ASSERT_TRUE(roster1.is_registered(client)) << "client is not registered";
    }

    roster1.deregister_client(6U);
    ASSERT_FALSE(roster1.is_registered(6U)) << "client is still registered";

    roster1.deregister_client(17U);
    ASSERT_FALSE(roster1.is_registered(17U)) << "client is still registered";

    roster1.deregister_client(10U);
    ASSERT_FALSE(roster1.is_registered(10U)) << "client is still registered";

    roster1.deregister_client(21U);
    ASSERT_FALSE(roster1.is_registered(21U)) << "client is still registered";
}

TEST(roster_test, get_client)
{
    rse::roster roster1;
    ASSERT_FALSE(roster1.is_finalised());
    rcs::client_id client = 1U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "aaa", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bbb", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster1.finalise()) << "finalisation failed";

    rce::old_player_id player1{rce::UniformNumber::FOUR, rce::TeamId::ALPHA};
    EXPECT_EQ(4U, roster1.get_client(player1)) << "client not found";

    rce::old_player_id player2{rce::UniformNumber::FOUR, rce::TeamId::BETA};
    EXPECT_EQ(15U, roster1.get_client(player2)) << "client not found";

    rce::old_player_id player3{rce::UniformNumber::NINE, rce::TeamId::ALPHA};
    EXPECT_EQ(9U, roster1.get_client(player3)) << "client not found";

    rce::old_player_id player4{rce::UniformNumber::NINE, rce::TeamId::BETA};
    EXPECT_EQ(20U, roster1.get_client(player4)) << "client not found";
}

TEST(roster_test, get_player)
{
    rse::roster roster1;
    ASSERT_FALSE(roster1.is_finalised());
    rcs::client_id client = 1U;
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "aaa", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bbb", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
	++client;
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster1.finalise()) << "finalisation failed";

    rce::old_player_id player1{rce::UniformNumber::SIX, rce::TeamId::ALPHA};
    EXPECT_EQ(player1, roster1.get_player(6U)) << "player not found";

    rce::old_player_id player2{rce::UniformNumber::SIX, rce::TeamId::BETA};
    EXPECT_EQ(player2, roster1.get_player(17U)) << "player not found";

    rce::old_player_id player3{rce::UniformNumber::TEN, rce::TeamId::ALPHA};
    EXPECT_EQ(player3, roster1.get_player(10U)) << "player not found";

    rce::old_player_id player4{rce::UniformNumber::TEN, rce::TeamId::BETA};
    EXPECT_EQ(player4, roster1.get_player(21U)) << "player not found";
}

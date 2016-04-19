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
    ASSERT_EQ(rse::roster::registration_result::team_slot_taken, register_client(1U, "blah", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
}

TEST(roster_test, registration_uniform_taken)
{
    rse::roster roster1;
    ASSERT_EQ(rse::roster::registration_result::success, register_client(1U, "foo", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    ASSERT_EQ(rse::roster::registration_result::uniform_taken, register_client(2U, "foo", rce::UniformNumber::ONE, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
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

TEST(roster_test, finalisation_basic)
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
    ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bar", rce::UniformNumber::ONE, rce::PlayerType::GOAL_KEEPER, roster1)) << "reg failed";
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::TWO, rce::UniformNumber::ELEVEN>())
    {
	++client;
	ASSERT_EQ(rse::roster::registration_result::success, register_client(client, "bar", uniform, rce::PlayerType::OUTFIELD, roster1)) << "reg failed";
    }
    ASSERT_EQ(rse::roster::finalisation_result::success, roster1.finalise()) << "finalisation failed";
}

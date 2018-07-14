#include <robocup2Dsim/common/action.hpp>
#include <robocup2Dsim/common/action.hh>
#include <robocup2Dsim/common/action.capnp.h>
#include <utility>
#include <capnp/message.h>
#include <gtest/gtest.h>
#include <turbo/memory/slab_allocator.hpp>
#include <turbo/memory/slab_allocator.hh>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <robocup2Dsim/engine/physics.hh>
#include <robocup2Dsim/engine/inventory.hh>

namespace rco = robocup2Dsim::common;
namespace ren = robocup2Dsim::engine;
namespace red = robocup2Dsim::engine::dynamics;
namespace rru = robocup2Dsim::runtime;

TEST(action_test, local_allocator_basic)
{
    EXPECT_TRUE(rru::update_local_db().component_allocator().in_configured_range(4U));
}

ren::physics_ptr<red::body> make_body(ren::physics& physics)
{
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_dynamicBody;
    body_def.linearDamping = 0.15f;
    body_def.angularDamping = 0.15f;
    body_def.position.Set(20.0, 20.0);
    body_def.angle = 0.0;
    ren::physics_ptr<red::body> result = physics.make_body(0U, body_def);
    ren::physics::mass_data mass;
    result->GetMassData(&mass);
    mass.I = 1.0f;
    result->SetMassData(&mass);
    return std::move(result);
}

rco::player_body make_player_body(ren::physics& physics)
{
    rco::player_body result{make_body(physics), make_body(physics), make_body(physics)};
    return std::move(result);
}

TEST(action_test, act_move_foot_forward_understock)
{
    ren::energy original_energy1{rco::MoveFootAction::MAX_COST / 2};
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(rco::MoveFootAction::MAX_VELOCITY);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x == new_position1.x && original_position1.y == new_position1.y)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_move_foot_backward_understock)
{
    ren::energy original_energy1{rco::MoveFootAction::MAX_COST / 2};
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(-rco::MoveFootAction::MAX_VELOCITY);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x == new_position1.x && original_position1.y == new_position1.y)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_move_foot_forward_basic)
{
    ren::energy original_energy1{rco::MoveFootAction::MAX_COST + 20};
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(rco::MoveFootAction::MAX_VELOCITY);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_move_foot_backward_basic)
{
    ren::energy original_energy1{rco::MoveFootAction::MAX_COST + 20};
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(-rco::MoveFootAction::MAX_VELOCITY);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_move_foot_forward_half_velocity)
{
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::MoveFootAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(rco::MoveFootAction::MAX_VELOCITY / 2);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::MoveFootAction::MAX_COST / 2, stock1.quantity) << "energy spent was not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_move_foot_backward_half_velocity)
{
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::MoveFootAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(-rco::MoveFootAction::MAX_VELOCITY / 2);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::MoveFootAction::MAX_COST / 2, stock1.quantity) << "energy spent was not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_move_foot_excess_forward_velocity)
{
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::MoveFootAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(rco::MoveFootAction::MAX_VELOCITY + 10);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_move_foot_excess_backward_velocity)
{
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST +20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::MoveFootAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(-rco::MoveFootAction::MAX_VELOCITY - 10);
    ren::physics::vec2 original_position1 = body1.foot->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.foot->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_run_forward_understock)
{
    ren::energy original_energy1{rco::RunAction::MAX_FORWARD_COST / 2};
    ren::inventory inventory1({0U, rco::RunAction::MAX_FORWARD_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_FORWARD_VELOCITY);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x == new_position1.x && original_position1.y == new_position1.y)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_run_backward_understock)
{
    ren::energy original_energy1{rco::RunAction::MAX_BACKWARD_COST / 2};
    ren::inventory inventory1({0U, rco::RunAction::MAX_BACKWARD_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_BACKWARD_VELOCITY);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x == new_position1.x && original_position1.y == new_position1.y)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_run_forward_basic)
{
    ren::energy original_energy1{rco::RunAction::MAX_FORWARD_COST + 20};
    ren::inventory inventory1({0U, rco::RunAction::MAX_FORWARD_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_FORWARD_VELOCITY);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_run_backward_basic)
{
    ren::energy original_energy1{rco::RunAction::MAX_BACKWARD_COST + 10};
    ren::inventory inventory1({0U, rco::RunAction::MAX_BACKWARD_COST + 10});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_BACKWARD_VELOCITY);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_run_forward_half_velocity)
{
    ren::inventory inventory1({0U, rco::RunAction::MAX_FORWARD_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::RunAction::MAX_FORWARD_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_FORWARD_VELOCITY / 2);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::RunAction::MAX_FORWARD_COST / 2, stock1.quantity) << "energy spent is not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_run_backward_half_velocity)
{
    ren::inventory inventory1({0U, rco::RunAction::MAX_BACKWARD_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::RunAction::MAX_BACKWARD_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_BACKWARD_VELOCITY / 2);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::RunAction::MAX_BACKWARD_COST / 2, stock1.quantity) << "energy spent is not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_run_forward_excess_velocity)
{
    ren::inventory inventory1({0U, rco::RunAction::MAX_FORWARD_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::RunAction::MAX_FORWARD_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_FORWARD_VELOCITY + 10);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_run_backward_excess_velocity)
{
    ren::inventory inventory1({0U, rco::RunAction::MAX_BACKWARD_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::RunAction::MAX_BACKWARD_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::RunAction::Builder action1 = arena1.initRoot<rco::RunAction>();
    action1.setVelocity(rco::RunAction::MAX_BACKWARD_VELOCITY - 10);
    ren::physics::vec2 original_position1 = body1.torso->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = body1.torso->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_head_antiwise_understock)
{
    ren::energy original_energy1{rco::TurnHeadAction::MAX_COST / 2};
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(rco::TurnHeadAction::MAX_VELOCITY);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(original_angle1 == new_angle1)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_turn_head_clockwise_understock)
{
    ren::energy original_energy1{rco::TurnHeadAction::MAX_COST / 2};
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(-rco::TurnHeadAction::MAX_VELOCITY);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(original_angle1 == new_angle1)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_turn_head_antiwise_basic)
{
    ren::energy original_energy1{rco::TurnHeadAction::MAX_COST + 20};
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(rco::TurnHeadAction::MAX_VELOCITY);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(original_angle1 < new_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_head_clockwise_basic)
{
    ren::energy original_energy1{rco::TurnHeadAction::MAX_COST + 20};
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(-rco::TurnHeadAction::MAX_VELOCITY);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(new_angle1 < original_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_head_antiwise_half_velocity)
{
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnHeadAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(rco::TurnHeadAction::MAX_VELOCITY / 2);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::TurnHeadAction::MAX_COST / 2, stock1.quantity) << "energy spent was not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(original_angle1 < new_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_head_clockwise_half_velocity)
{
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnHeadAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(-rco::TurnHeadAction::MAX_VELOCITY / 2);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::TurnHeadAction::MAX_COST / 2, stock1.quantity) << "energy spent was not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(new_angle1 < original_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_head_antiwise_excess_velocity)
{
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnHeadAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(rco::TurnHeadAction::MAX_VELOCITY + 20);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(original_angle1 < new_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_head_clockwise_excess_velocity)
{
    ren::inventory inventory1({0U, rco::TurnHeadAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnHeadAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnHeadAction::Builder action1 = arena1.initRoot<rco::TurnHeadAction>();
    action1.setVelocity(-rco::TurnHeadAction::MAX_VELOCITY - 20);
    auto original_angle1 = body1.head->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_angle1 = body1.head->GetAngle();
    EXPECT_TRUE(new_angle1 < original_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_torso_antiwise_understock)
{
    ren::energy original_energy1{rco::TurnTorsoAction::MAX_COST / 2};
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(rco::TurnTorsoAction::MAX_VELOCITY);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(original_torso_angle1 == new_torso_angle1 && original_foot_angle1 == new_foot_angle1)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_turn_torso_clockwise_understock)
{
    ren::energy original_energy1{rco::TurnTorsoAction::MAX_COST / 2};
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(-rco::TurnTorsoAction::MAX_VELOCITY);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::understock,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "act succeeded when stock was insufficient";
    EXPECT_EQ(original_energy1.quantity, stock1.quantity) << "act failed but energy was still spent";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(original_torso_angle1 == new_torso_angle1 && original_foot_angle1 == new_foot_angle1)
	    << "unsuccessful act affected the simulation";
}

TEST(action_test, act_turn_torso_antiwise_basic)
{
    ren::energy original_energy1{rco::TurnTorsoAction::MAX_COST + 20};
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(rco::TurnTorsoAction::MAX_VELOCITY);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(original_torso_angle1 < new_torso_angle1 && original_foot_angle1 < new_foot_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_torso_clockwise_basic)
{
    ren::energy original_energy1{rco::TurnTorsoAction::MAX_COST + 20};
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(-rco::TurnTorsoAction::MAX_VELOCITY);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(new_torso_angle1 < original_torso_angle1 && new_foot_angle1 < original_foot_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_torso_antiwise_half_velocity)
{
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnTorsoAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(rco::TurnTorsoAction::MAX_VELOCITY / 2);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::TurnTorsoAction::MAX_COST / 2, stock1.quantity) << "energy spent was not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(original_torso_angle1 < new_torso_angle1 && original_foot_angle1 < new_foot_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_torso_clockwise_half_velocity)
{
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnTorsoAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(-rco::TurnTorsoAction::MAX_VELOCITY / 2);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(rco::TurnTorsoAction::MAX_COST / 2, stock1.quantity) << "energy spent was not proportional to the specified velocity";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(new_torso_angle1 < original_torso_angle1 && new_foot_angle1 < original_foot_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_torso_antiwise_excess_velocity)
{
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnTorsoAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(rco::TurnTorsoAction::MAX_VELOCITY + 20);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(original_torso_angle1 < new_torso_angle1 && original_foot_angle1 < new_foot_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, act_turn_torso_clockwise_excess_velocity)
{
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST + 20});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{rco::TurnTorsoAction::MAX_COST};
    rco::player_body body1 = make_player_body(physics1);
    capnp::MallocMessageBuilder arena1;
    rco::TurnTorsoAction::Builder action1 = arena1.initRoot<rco::TurnTorsoAction>();
    action1.setVelocity(-rco::TurnTorsoAction::MAX_VELOCITY - 20);
    auto original_torso_angle1 = body1.torso->GetAngle();
    auto original_foot_angle1 = body1.foot->GetAngle();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, body1, action1.asReader()))
	    << "failed to act";
    EXPECT_EQ(0U, stock1.quantity) << "excessive velocity was not capped to the defined maximum";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    auto new_torso_angle1 = body1.torso->GetAngle();
    auto new_foot_angle1 = body1.foot->GetAngle();
    EXPECT_TRUE(new_torso_angle1 < original_torso_angle1 && new_foot_angle1 < original_foot_angle1)
	    << "action had no effect on simulation";
}

TEST(action_test, vector_act_invalid_player)
{
    ren::energy original_energy1{rco::TurnTorsoAction::MAX_COST};
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    std::array<ren::energy, 2U> stock1{original_energy1.quantity, original_energy1.quantity};
    std::array<rco::player_body, 2U> body1{make_player_body(physics1), make_player_body(physics1)};
    capnp::MallocMessageBuilder arena1;
    rco::PlayerAction::Builder action1 = arena1.initRoot<rco::PlayerAction>();
    std::vector<rco::client_action> action_list1;
    action_list1.emplace_back(action1.asReader(), body1.max_size());
    action_list1.emplace_back(action1.asReader(), body1.max_size() + 1);
    std::vector<ren::inventory::spend_result> result_list1;
    act(inventory1, stock1, physics1, body1, action_list1, result_list1);
    for (ren::inventory::spend_result result: result_list1)
    {
	EXPECT_EQ(ren::inventory::spend_result::understock, result) << "vector act with invalid player_id succeeded";
    }
}

TEST(action_test, vector_act_understock)
{
    ren::energy original_energy1{rco::RunAction::MAX_FORWARD_COST / 2U};
    ren::inventory inventory1({0U, rco::MoveFootAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    std::array<ren::energy, 2U> stock1{original_energy1.quantity, original_energy1.quantity};
    std::array<rco::player_body, 2U> body1{make_player_body(physics1), make_player_body(physics1)};
    std::vector<rco::client_action> action_list1;
    capnp::MallocMessageBuilder arena1a;
    rco::PlayerAction::Builder action1a = arena1a.initRoot<rco::PlayerAction>();
    rco::RunAction::Builder run1a =  action1a.getAction().initRun();
    run1a.setVelocity(rco::RunAction::MAX_FORWARD_VELOCITY);
    action_list1.emplace_back(action1a.asReader(), 0U);
    capnp::MallocMessageBuilder arena1b;
    rco::PlayerAction::Builder action1b = arena1b.initRoot<rco::PlayerAction>();
    rco::MoveFootAction::Builder move1b =  action1b.getAction().initMoveFoot();
    move1b.setVelocity(rco::MoveFootAction::MAX_VELOCITY);
    action_list1.emplace_back(action1b.asReader(), 1U);
    std::vector<ren::inventory::spend_result> result_list1;
    act(inventory1, stock1, physics1, body1, action_list1, result_list1);
    for (ren::inventory::spend_result result: result_list1)
    {
	EXPECT_EQ(ren::inventory::spend_result::understock, result) << "vector act succeeded with insufficient stock";
    }
}

TEST(action_test, vector_act_half_success)
{
    ASSERT_TRUE(rco::MoveFootAction::MAX_COST < rco::RunAction::MAX_BACKWARD_COST) << "Wrong action mix picked";
    ren::energy original_energy1{rco::MoveFootAction::MAX_COST};
    ren::inventory inventory1({0U, rco::RunAction::MAX_BACKWARD_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    std::array<ren::energy, 2U> stock1{original_energy1.quantity, original_energy1.quantity};
    std::array<rco::player_body, 2U> body1{make_player_body(physics1), make_player_body(physics1)};
    std::vector<rco::client_action> action_list1;
    capnp::MallocMessageBuilder arena1a;
    rco::PlayerAction::Builder action1a = arena1a.initRoot<rco::PlayerAction>();
    rco::RunAction::Builder run1a =  action1a.getAction().initRun();
    run1a.setVelocity(rco::RunAction::MAX_BACKWARD_VELOCITY);
    action_list1.emplace_back(action1a.asReader(), 0U);
    capnp::MallocMessageBuilder arena1b;
    rco::PlayerAction::Builder action1b = arena1b.initRoot<rco::PlayerAction>();
    rco::MoveFootAction::Builder move1b =  action1b.getAction().initMoveFoot();
    move1b.setVelocity(rco::MoveFootAction::MAX_VELOCITY);
    action_list1.emplace_back(action1b.asReader(), 1U);
    std::vector<ren::inventory::spend_result> result_list1;
    act(inventory1, stock1, physics1, body1, action_list1, result_list1);
    EXPECT_EQ(ren::inventory::spend_result::understock, result_list1[0]) << "vector act succeeded on action with insufficient stock";
    EXPECT_EQ(ren::inventory::spend_result::success, result_list1[1]) << "vector act failed on action with sufficient stock";
}

TEST(action_test, vector_act_all_success)
{
    ASSERT_TRUE(rco::TurnHeadAction::MAX_COST < rco::TurnTorsoAction::MAX_COST) << "Wrong action mix picked";
    ren::energy original_energy1{rco::TurnTorsoAction::MAX_COST};
    ren::inventory inventory1({0U, rco::TurnTorsoAction::MAX_COST});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    std::array<ren::energy, 2U> stock1{original_energy1.quantity, original_energy1.quantity};
    std::array<rco::player_body, 2U> body1{make_player_body(physics1), make_player_body(physics1)};
    std::vector<rco::client_action> action_list1;
    capnp::MallocMessageBuilder arena1a;
    rco::PlayerAction::Builder action1a = arena1a.initRoot<rco::PlayerAction>();
    rco::TurnTorsoAction::Builder turn_torso1a =  action1a.getAction().initTurnTorso();
    turn_torso1a.setVelocity(rco::TurnTorsoAction::MAX_VELOCITY);
    action_list1.emplace_back(action1a.asReader(), 0U);
    capnp::MallocMessageBuilder arena1b;
    rco::PlayerAction::Builder action1b = arena1b.initRoot<rco::PlayerAction>();
    rco::TurnHeadAction::Builder turn_head1b =  action1b.getAction().initTurnHead();
    turn_head1b.setVelocity(rco::TurnHeadAction::MAX_VELOCITY);
    action_list1.emplace_back(action1b.asReader(), 1U);
    std::vector<ren::inventory::spend_result> result_list1;
    act(inventory1, stock1, physics1, body1, action_list1, result_list1);
    EXPECT_EQ(ren::inventory::spend_result::success, result_list1[0]) << "vector act failed on action with sufficient stock";
    EXPECT_EQ(ren::inventory::spend_result::success, result_list1[1]) << "vector act failed on action with sufficient stock";
}

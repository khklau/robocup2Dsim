#include <robocup2Dsim/common/action.hpp>
#include <robocup2Dsim/common/action.capnp.h>
#include <capnp/message.h>
#include <gtest/gtest.h>
#include <turbo/memory/slab_allocator.hpp>
#include <turbo/memory/slab_allocator.hxx>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <robocup2Dsim/engine/physics.hxx>
#include <robocup2Dsim/engine/inventory.hxx>

namespace rco = robocup2Dsim::common;
namespace ren = robocup2Dsim::engine;
namespace red = robocup2Dsim::engine::dynamics;
namespace rru = robocup2Dsim::runtime;

TEST(action_test, local_allocator_basic)
{
    EXPECT_TRUE(rru::update_local_db().component_allocator().in_configured_range(4U));
}

TEST(action_test, act_move_foot_basic)
{
    ren::energy original_energy1{rco::MoveFootAction::MAX_COST};
    ren::inventory inventory1({0U, 100U});
    ren::physics physics1(ren::physics::vec2(0.0, 0.0), {8U, 4U});
    ren::energy stock1{original_energy1.quantity};
    ren::physics::body_def body_def1;
    body_def1.type = b2_dynamicBody;
    body_def1.position.Set(20.0, 20.0);
    body_def1.angle = 0.0;
    ren::physics_ptr<red::body> torso1 = physics1.make_body(0U, body_def1);
    ren::physics_ptr<red::body> foot1 = physics1.make_body(0U, body_def1);
    capnp::MallocMessageBuilder arena1;
    rco::MoveFootAction::Builder action1 = arena1.initRoot<rco::MoveFootAction>();
    action1.setVelocity(rco::MoveFootAction::MAX_VELOCITY);
    ren::physics::vec2 original_position1 = foot1->GetPosition();
    EXPECT_EQ(ren::inventory::spend_result::success,
	    rco::act(inventory1, stock1, physics1, *torso1, *foot1, action1.asReader()))
	    << "failed to act";
    EXPECT_NE(original_energy1.quantity, stock1.quantity) << "energy stock quantity is the same after successful action";
    physics1.step(10.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void { });
    ren::physics::vec2 new_position1 = foot1->GetPosition();
    EXPECT_TRUE(original_position1.x != new_position1.x || original_position1.y != new_position1.y)
	    << "action had no effect on simulation";
}

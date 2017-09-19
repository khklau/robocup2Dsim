#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/engine/physics.hxx>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <gtest/gtest.h>
#include <robocup2Dsim/runtime/db_access.hpp>

namespace ren = robocup2Dsim::engine;
namespace red = robocup2Dsim::engine::dynamics;
namespace rru = robocup2Dsim::runtime;

static const float deg2rad = 0.0174532925199432957f;
static const float rad2deg = 57.295779513082320876f;

TEST(db_access_test, local_allocator_basic)
{
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::register_system(rru::update_local_db(), std::move(physics1));
}

TEST(db_access_test, make_body_basic)
{
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics::body_def body_def1;
    body_def1.type = b2_dynamicBody;
    body_def1.position.Set(10.0, 20.0);
    body_def1.angle = 0.0;
    ren::physics_ptr<red::body> body1 = physics1->make_body(0U, body_def1);
    bool actual1(body1);
    EXPECT_TRUE(actual1) << "body pointer is nullptr";
}

TEST(db_access_test, make_fixture_basic)
{
    enum class category1 : std::uint8_t
    {
	red,
	green,
	blue,
	yellow,
	max = yellow
    };
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics::body_def body_def1;
    ren::physics_ptr<red::body> body1 = physics1->make_body(1U, body_def1);
    ren::contact_config<category1> config1(ren::contact_result::collide, ren::contact_result::pass_over);
    config1.set(category1::yellow, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def1 = physics1->make_fixture_def(1U, category1::green, config1);
    ren::physics::circle_shape shape1;
    shape1.m_p.Set(0, 0);
    shape1.m_radius = 1;
    fixture_def1.shape = &shape1;
    physics1->make_fixture(1U, body1, fixture_def1);
}

TEST(db_access_test, make_revolute_joint_basic)
{
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics::body_def body_def1;
    body_def1.type = b2_dynamicBody;
    body_def1.position.Set(10.0, 20.0);
    body_def1.angle = 0.0;
    ren::physics_ptr<red::body> body1a = physics1->make_body(0U, body_def1);
    ren::physics_ptr<red::body> body1b = physics1->make_body(0U, body_def1);
    ren::physics::revolute_joint_def joint_def1;
    joint_def1.bodyA = body1a.get();
    joint_def1.bodyB = body1b.get();
    joint_def1.collideConnected = false;
    joint_def1.localAnchorA.Set(0, 0);
    joint_def1.localAnchorB.Set(0, 0);
    joint_def1.referenceAngle = 0;
    joint_def1.enableLimit = true;
    joint_def1.lowerAngle = -60 * deg2rad;
    joint_def1.upperAngle = 60 * deg2rad;
    physics1->make_joint(0U, joint_def1);
}

TEST(db_access_test, make_prismatic_joint_basic)
{
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics::body_def body_def1;
    body_def1.type = b2_dynamicBody;
    body_def1.position.Set(10.0, 20.0);
    body_def1.angle = 0.0;
    ren::physics_ptr<red::body> body1a = physics1->make_body(0U, body_def1);
    ren::physics_ptr<red::body> body1b = physics1->make_body(0U, body_def1);
    ren::physics::prismatic_joint_def joint_def1;
    joint_def1.bodyA = body1a.get();
    joint_def1.bodyB = body1b.get();
    joint_def1.localAnchorA.Set(0, 0);
    joint_def1.localAnchorB.Set(0, 0);
    joint_def1.localAxisA.Set(1, 0);
    joint_def1.localAxisA.Normalize();
    joint_def1.enableLimit = true;
    joint_def1.lowerTranslation = 0;
    joint_def1.upperTranslation = 2;
    physics1->make_joint(0U, joint_def1);
}

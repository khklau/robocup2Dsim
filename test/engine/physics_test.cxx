#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/engine/physics.hh>
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

TEST(physics_test, register_instance)
{
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics* physics1_ptr = physics1.get();
    ren::register_system(rru::update_local_db(), std::move(physics1));

    EXPECT_NO_THROW(ren::select_physics_instance(rru::select_local_db()))
            << "Registered physics instance is not accessible";
    EXPECT_NO_THROW(ren::update_physics_instance(rru::update_local_db()))
            << "Registered physics instance is not accessible";
    EXPECT_EQ(physics1_ptr, &(ren::select_physics_instance(rru::select_local_db())))
            << "Physics instances returned is not the instance registered";
    EXPECT_EQ(physics1_ptr, &(ren::update_physics_instance(rru::update_local_db())))
            << "Physics instances returned is not the instance registered";

    std::unique_ptr<ren::physics> physics2(new ren::physics());
    ren::physics* physics2_ptr = physics2.get();
    std::unique_ptr<ren::physics> physics3 = ren::register_system(rru::update_local_db(), std::move(physics2));

    EXPECT_EQ(physics1_ptr, physics3.get())
            << "Displaced physics instance is not the original registered";
    EXPECT_NO_THROW(ren::select_physics_instance(rru::select_local_db()))
            << "Registered physics instance is not accessible";
    EXPECT_NO_THROW(ren::update_physics_instance(rru::update_local_db()))
            << "Registered physics instance is not accessible";
    EXPECT_EQ(physics2_ptr, &(ren::select_physics_instance(rru::select_local_db())))
            << "Physics instances returned is not the instance registered";
    EXPECT_EQ(physics2_ptr, &(ren::update_physics_instance(rru::update_local_db())))
            << "Physics instances returned is not the instance registered";
}

TEST(physics_test, make_body_basic)
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

TEST(physics_test, make_fixture_basic)
{
    enum class category1 : std::uint8_t
    {
	red,
	green,
	blue,
	yellow,
	max = yellow
    };
    enum fixture_name : ren::physics::fixture_id_type
    {
	ball
    };
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics::body_def body_def1;
    ren::physics_ptr<red::body> body1 = physics1->make_body(1U, body_def1);
    ren::contact_config<category1> config1(ren::contact_result::collide, ren::contact_result::pass_over);
    config1.set(category1::yellow, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def1 = physics1->make_fixture_def(1U, ball, category1::green, config1);
    ren::physics::circle_shape shape1;
    shape1.m_p.Set(0, 0);
    shape1.m_radius = 1;
    fixture_def1.shape = &shape1;
    physics1->make_fixture(*body1, fixture_def1);
}

TEST(physics_test, make_revolute_joint_basic)
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
    physics1->make_joint(joint_def1);
}

TEST(physics_test, make_prismatic_joint_basic)
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
    physics1->make_joint(joint_def1);
}

TEST(physics_test, step_basic)
{
    enum class category1 : std::uint8_t
    {
	red,
	green,
	blue,
	yellow,
	max = yellow
    };
    enum fixture_name : ren::physics::fixture_id_type
    {
	ball
    };
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::physics::body_def body_def1;
    ren::physics_ptr<red::body> body1 = physics1->make_body(1U, body_def1);
    ren::contact_config<category1> config1(ren::contact_result::collide, ren::contact_result::pass_over);
    config1.set(category1::yellow, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def1 = physics1->make_fixture_def(1U, ball, category1::green, config1);
    ren::physics::circle_shape shape1;
    shape1.m_p.Set(0, 0);
    shape1.m_radius = 1;
    fixture_def1.shape = &shape1;
    physics1->make_fixture(*body1, fixture_def1);
    std::size_t collision_count = 0U;
    std::size_t separation_count = 0U;
    physics1->step(1.0,
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void
	    {
		++collision_count;
	    },
	    [&](const ren::physics::contact_participant&, const ren::physics::contact_participant&) -> void
	    {
		++separation_count;
	    });
}

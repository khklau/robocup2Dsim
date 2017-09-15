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

#include "ball.hpp"
#include <utility>
#include <robocup2Dsim/common/entity.hpp>

namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace common {

ball_components make_ball(robocup2Dsim::runtime::ecs_db& db, const ren::physics::vec2& position)
{
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    body_def.angle = 0;
    body_def.linearDamping = 0.15f;
    body_def.angularDamping = 0.15f;
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity("ball");
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    ren::physics::circle_shape shape;
    shape.m_p.Set(0, 0);
    shape.m_radius = 1;

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::marker_sensor, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::ball),
	    entity::collision_category::ball,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(*body, fixture_def);

    ball_components result{std::move(body)};
    return std::move(result);
}

} // namespace common
} // robocup2Dsim

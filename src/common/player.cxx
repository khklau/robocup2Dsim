#include "player.hpp"
#include <utility>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/engine/math.hpp>

namespace rem = robocup2Dsim::engine::math;
namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace common {

ren::physics_ptr<ren::dynamics::body> make_torso(
	rru::ecs_db& db,
	const std::string& player_name,
	const ren::physics::vec2& position,
	const std::uint16_t angle_degree)
{
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_dynamicBody;
    body_def.linearDamping = 0.15f;
    body_def.angularDamping = 0.15f;
    std::string torso_name(player_name);
    torso_name.append(" torso");
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity(torso_name);
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    ren::physics::mass_data mass;
    body->GetMassData(&mass);
    mass.I = 1.0f;
    body->SetMassData(&mass);
    body->SetTransform(position, angle_degree * rem::deg2rad);

    std::array<ren::physics::vec2, 8> vertices;
    vertices[0].Set(1, -1);
    vertices[1].Set(1, 1);
    vertices[2].Set(0.5, 2);
    vertices[3].Set(-0.5, 2);
    vertices[4].Set(-1, 1);
    vertices[5].Set(-1, -1);
    vertices[6].Set(-0.5, -2);
    vertices[7].Set(0.5, -2);
    ren::physics::polygon_shape shape;
    shape.Set(vertices.data(), vertices.max_size());

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::ball_sensor, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::marker_sensor, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::torso),
	    entity::collision_category::player_body,
	    collision_config);
    fixture_def.shape = &shape;
    fixture_def.density = 4.0f;
    physics.make_fixture(*body, fixture_def);

    return std::move(body);
}

player_components make_player(
	rru::ecs_db& db,
	const std::string& name,
	const ren::physics::vec2& position,
	const std::uint16_t angle_degree)
{
    ren::physics_ptr<ren::dynamics::body> torso = make_torso(db, name, position, angle_degree);
    player_components result{std::move(torso)};
}

} // namespace common
} // namespace robocup2Dsim

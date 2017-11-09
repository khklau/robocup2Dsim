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

void make_vision(
	ren::physics& physics,
	rru::ecs_db::entity_table_type::key_type entity_id,
	ren::dynamics::body& body,
	std::size_t vision_radius,
	const std::uint16_t arc_degree,
	const std::int16_t offset_degree)
{
    ren::physics::polygon_shape shape;
    std::array<ren::physics::vec2, 8> vertices;
    vertices[0].Set(0, 0);
    for (std::size_t vertex = 0; vertex < (vertices.max_size() - 1); ++vertex)
    {
	    float angle = ((vertex / (vertices.max_size() - 2.0) * arc_degree) + offset_degree) * rem::deg2rad;
	    vertices[vertex + 1].Set(vision_radius * cosf(angle), vision_radius * sinf(angle));
    }
    shape.Set(vertices.data(), vertices.max_size());

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::player_sensor, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::ball_sensor, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::vision),
	    entity::collision_category::player_sensor,
	    collision_config);
    fixture_def.shape = &shape;
    fixture_def.density = 0;
    fixture_def.isSensor = true;
    physics.make_fixture(body, fixture_def);
}

ren::physics_ptr<ren::dynamics::body> make_head(
	rru::ecs_db& db,
	const std::string& player_name,
	const ren::physics::vec2& position,
	const std::uint16_t angle_degree,
	std::size_t vision_radius,
	const std::uint16_t vision_degree)
{
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_dynamicBody;
    body_def.linearDamping = 0.15f;
    body_def.angularDamping = 0.15f;
    std::string head_name(player_name);
    head_name.append(" head");
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity(head_name);
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    ren::physics::mass_data mass;
    body->GetMassData(&mass);
    mass.I = 1.0f;
    body->SetMassData(&mass);
    body->SetTransform(position, angle_degree * rem::deg2rad);

    ren::physics::circle_shape shape;
    shape.m_p.Set(0, 0);
    shape.m_radius = 1;

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::ball_sensor, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::marker_sensor, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::head),
	    entity::collision_category::player_body,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(*body, fixture_def);

    const std::size_t arc_degree = vision_degree / 4;
    make_vision(physics, entity_id, *body, vision_radius, arc_degree, arc_degree * -2);
    make_vision(physics, entity_id, *body, vision_radius, arc_degree, arc_degree * -1);
    make_vision(physics, entity_id, *body, vision_radius, arc_degree, 0);
    make_vision(physics, entity_id, *body, vision_radius, arc_degree, arc_degree);

    return std::move(body);
}

ren::physics_ptr<ren::dynamics::body> make_foot(
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
    std::string foot_name(player_name);
    foot_name.append(" foot");
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity(foot_name);
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    ren::physics::mass_data mass;
    body->GetMassData(&mass);
    mass.I = 1.0f;
    body->SetMassData(&mass);
    body->SetTransform(position, angle_degree * rem::deg2rad);

    std::array<ren::physics::vec2, 4> vertices;
    vertices[0].Set(1, 0.5);
    vertices[1].Set(1, -0.5);
    vertices[2].Set(-1, -0.5);
    vertices[3].Set(-1, 0.5);
    ren::physics::polygon_shape shape;
    shape.Set(vertices.data(), vertices.max_size());

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::ball_sensor, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::marker_sensor, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::foot),
	    entity::collision_category::player_body,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(*body, fixture_def);

    return std::move(body);
}

void make_neck(
	ren::physics& physics,
	ren::dynamics::body& torso,
	ren::dynamics::body& head,
	const std::int16_t min_angle_degree,
	const std::int16_t max_angle_degree)
{
    ren::physics::revolute_joint_def joint_def;
    joint_def.bodyA = &torso;
    joint_def.bodyB = &head;
    joint_def.collideConnected = false;
    joint_def.localAnchorA.Set(0, 0);
    joint_def.localAnchorB.Set(0, 0);
    joint_def.referenceAngle = 0;
    joint_def.enableLimit = true;
    joint_def.lowerAngle = min_angle_degree * rem::deg2rad;
    joint_def.upperAngle = max_angle_degree * rem::deg2rad;
    physics.make_joint(joint_def);
}

void make_hip(
	ren::physics& physics,
	ren::dynamics::body& torso,
	ren::dynamics::body& foot,
	const std::int16_t min_flex,
	const std::int16_t max_flex)
{
    ren::physics::prismatic_joint_def joint_def;
    joint_def.bodyA = &torso;
    joint_def.bodyB = &foot;
    joint_def.localAnchorA.Set(0, 0);
    joint_def.localAnchorB.Set(0, 0);
    joint_def.localAxisA.Set(1, 0);
    joint_def.localAxisA.Normalize();
    joint_def.enableLimit = true;
    joint_def.lowerTranslation = min_flex;
    joint_def.upperTranslation = max_flex;
    physics.make_joint(joint_def);
}

player_components make_player(
	rru::ecs_db& db,
	const std::string& name,
	const ren::physics::vec2& position,
	const std::uint16_t angle_degree)
{
    ren::physics_ptr<ren::dynamics::body> torso = make_torso(db, name, position, angle_degree);
    ren::physics_ptr<ren::dynamics::body> head = make_head(db, name, position, angle_degree, 360, 120);
    ren::physics_ptr<ren::dynamics::body> foot = make_foot(db, name, position, angle_degree);
    make_neck(ren::update_physics_instance(db), *torso, *head, -60, 60);
    make_hip(ren::update_physics_instance(db), *torso, *foot, 0, 2);
    player_components result{
	    std::move(torso),
	    std::move(head),
	    std::move(foot)};
    return std::move(result);
}

} // namespace common
} // namespace robocup2Dsim

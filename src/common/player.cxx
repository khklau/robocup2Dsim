#include "player.hpp"
#include <utility>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/engine/math.hpp>
#include <robocup2Dsim/engine/physics.hh>

namespace rem = robocup2Dsim::engine::math;
namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace common {

player_body::~player_body()
{
    ren::physics* engine = static_cast<ren::physics*>(torso.GetUserData());
     if (engine != nullptr)
     {
	 engine->destroy_body(&torso);
	 engine->destroy_body(&head);
	 engine->destroy_body(&foot);
     }
}

void make_torso(
        ren::dynamics::body& body,
	rru::ecs_db& db,
	const entity::player_id id,
	const ren::physics::vec2& position,
	const std::uint16_t angle_degree)
{
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_dynamicBody;
    body_def.linearDamping = 0.15f;
    body_def.angularDamping = 0.15f;
    std::string torso_name("player ");
    torso_name.append(std::to_string(id));
    torso_name.append(" torso");
    db.insert_entity(torso_name);
    physics.make_body(id, body_def, body);

    ren::physics::mass_data mass;
    body.GetMassData(&mass);
    mass.I = 1.0f;
    body.SetMassData(&mass);
    body.SetTransform(position, angle_degree * rem::deg2rad);

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
	    id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::torso),
	    entity::collision_category::player_body,
	    collision_config);
    fixture_def.shape = &shape;
    fixture_def.density = 4.0f;
    physics.make_fixture(body, fixture_def);
}

void make_vision(
	ren::physics& physics,
	const entity::player_id id,
	ren::dynamics::body& body,
	const std::size_t vision_radius,
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
	    id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::vision),
	    entity::collision_category::player_sensor,
	    collision_config);
    fixture_def.shape = &shape;
    fixture_def.density = 0;
    fixture_def.isSensor = true;
    physics.make_fixture(body, fixture_def);
}

void make_head(
        ren::dynamics::body& body,
	rru::ecs_db& db,
	const entity::player_id id,
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
    std::string head_name("player ");
    head_name.append(std::to_string(id));
    head_name.append(" head");
    db.insert_entity(head_name);
    physics.make_body(id, body_def, body);

    ren::physics::mass_data mass;
    body.GetMassData(&mass);
    mass.I = 1.0f;
    body.SetMassData(&mass);
    body.SetTransform(position, angle_degree * rem::deg2rad);

    ren::physics::circle_shape shape;
    shape.m_p.Set(0, 0);
    shape.m_radius = 1;

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::ball_sensor, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::marker_sensor, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::head),
	    entity::collision_category::player_body,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(body, fixture_def);

    const std::size_t arc_degree = vision_degree / 4;
    make_vision(physics, id, body, vision_radius, arc_degree, arc_degree * -2);
    make_vision(physics, id, body, vision_radius, arc_degree, arc_degree * -1);
    make_vision(physics, id, body, vision_radius, arc_degree, 0);
    make_vision(physics, id, body, vision_radius, arc_degree, arc_degree);
}

void make_foot(
        ren::dynamics::body& body,
	rru::ecs_db& db,
	const entity::player_id id,
	const ren::physics::vec2& position,
	const std::uint16_t angle_degree)
{
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_dynamicBody;
    body_def.linearDamping = 0.15f;
    body_def.angularDamping = 0.15f;
    std::string foot_name("player ");
    foot_name.append(std::to_string(id));
    foot_name.append(" foot");
    db.insert_entity(foot_name);
    physics.make_body(id, body_def, body);

    ren::physics::mass_data mass;
    body.GetMassData(&mass);
    mass.I = 1.0f;
    body.SetMassData(&mass);
    body.SetTransform(position, angle_degree * rem::deg2rad);

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
	    id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(entity::fixture_name::foot),
	    entity::collision_category::player_body,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(body, fixture_def);
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

void init_player(
        player_body& output,
	robocup2Dsim::runtime::ecs_db& db,
	const entity::player_id id,
	const robocup2Dsim::engine::physics::vec2& position,
	const std::uint16_t angle_degree)
{
    make_torso(output.torso, db, id, position, angle_degree);
    make_head(output.head, db, id, position, angle_degree, 360, 120);
    make_foot(output.foot, db, id, position, angle_degree);
    make_neck(ren::update_physics_instance(db), output.torso, output.head, -60, 60);
    make_hip(ren::update_physics_instance(db), output.torso, output.foot, 0, 2);
}

} // namespace common
} // namespace robocup2Dsim

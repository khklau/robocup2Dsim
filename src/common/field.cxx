#include "field.hpp"
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/common/field.capnp.h>

namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace common {

namespace {

void make_back_net(
	ren::physics& physics,
	rru::ecs_db::entity_table_type::key_type entity_id,
	ren::dynamics::body& body,
	Side side,
	entity::fixture_name fixture_name)
{
    std::array<ren::physics::vec2, 2> vertices;
    switch (side)
    {
	case Side::LEFT:
	    vertices[0].Set(-16.0, 32.0);
	    vertices[1].Set(-16.0, -32.0);
	    break;
	case Side::RIGHT:
	    vertices[0].Set(16.0, -32.0);
	    vertices[1].Set(16.0, 32.0);
	    break;
    }
    ren::physics::edge_shape shape;
    shape.Set(vertices[0], vertices[1]);
    shape.m_hasVertex0 = false;
    shape.m_hasVertex3 = false;

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(fixture_name),
	    entity::collision_category::goal_net,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(body, fixture_def);
}

void make_side_net(
	ren::physics& physics,
	rru::ecs_db::entity_table_type::key_type entity_id,
	ren::dynamics::body& body,
	Side side,
	entity::fixture_name fixture_name)
{
    std::array<ren::physics::vec2, 2> vertices;
    switch (side)
    {
	case Side::LEFT:
	    if (fixture_name == entity::fixture_name::top_net)
	    {
		vertices[0].Set(-16.0, 32.0);
		vertices[1].Set(-0.5, 32.0);
	    }
	    else if (fixture_name == entity::fixture_name::bottom_net)
	    {
		vertices[0].Set(-16.0, -32.0);
		vertices[1].Set(-0.5, -32.0);
	    }
	    break;
	case Side::RIGHT:
	    if (fixture_name == entity::fixture_name::top_net)
	    {
		vertices[0].Set(0.5, 32.0);
		vertices[1].Set(16.0, 32.0);
	    }
	    else if (fixture_name == entity::fixture_name::bottom_net)
	    {
		vertices[0].Set(0.5, -32.0);
		vertices[1].Set(16.0, -32.0);
	    }
	    break;
    }
    ren::physics::edge_shape shape;
    shape.Set(vertices[0], vertices[1]);
    shape.m_hasVertex0 = false;
    shape.m_hasVertex3 = false;

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(fixture_name),
	    entity::collision_category::goal_net,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(body, fixture_def);
}

void make_post(
	ren::physics& physics,
	rru::ecs_db::entity_table_type::key_type entity_id,
	ren::dynamics::body& body,
	Side,
	entity::fixture_name fixture_name)
{

    ren::physics::circle_shape shape;
    if (fixture_name == entity::fixture_name::top_net)
    {
	shape.m_p.Set(0.0, 32.0);
    }
    else if (fixture_name == entity::fixture_name::bottom_net)
    {
	shape.m_p.Set(0.0, -32.0);
    }
    shape.m_radius = 0.5;

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::collide, ren::contact_result::pass_over);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(fixture_name),
	    entity::collision_category::goal_post,
	    collision_config);
    fixture_def.shape = &shape;
    physics.make_fixture(body, fixture_def);
}

void make_ball_sensor(
	ren::physics& physics,
	rru::ecs_db::entity_table_type::key_type entity_id,
	ren::dynamics::body& body,
	const std::array<ren::physics::vec2, 4>& vertices,
	entity::fixture_name fixture_name)
{
    ren::physics::polygon_shape shape;
    shape.Set(vertices.data(), vertices.max_size());

    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::pass_over, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::ball, ren::contact_result::collide);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(fixture_name),
	    entity::collision_category::ball_sensor,
	    collision_config);
    fixture_def.shape = &shape;
    fixture_def.density = 0;
    fixture_def.isSensor = true;
    physics.make_fixture(body, fixture_def);
}

ren::physics_ptr<ren::dynamics::body> make_goal(
	rru::ecs_db& db,
	Side side,
	const ren::physics::vec2& position)
{
    const char* entity_name = nullptr;
    std::array<ren::physics::vec2, 4> vertices;
    switch (side)
    {
	case Side::LEFT:
	    entity_name = "left goal";
	    vertices[0].Set(-16.0, 32.0);
	    vertices[1].Set(-2.0, 32.0);
	    vertices[2].Set(-2.0, -32.0);
	    vertices[3].Set(-16.0, -32.0);
	    break;
	case Side::RIGHT:
	    entity_name = "right goal";
	    vertices[0].Set(16.0, -32.0);
	    vertices[1].Set(2.0, -32.0);
	    vertices[2].Set(2.0, 32.0);
	    vertices[3].Set(16.0, 32.0);
	    break;
    }

    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_staticBody;
    body_def.position.Set(position.x, position.y);
    body_def.angle = 0;
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity(entity_name);
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    make_back_net(physics, entity_id, *body, side, entity::fixture_name::back_net);
    make_side_net(physics, entity_id, *body, side, entity::fixture_name::top_net);
    make_side_net(physics, entity_id, *body, side, entity::fixture_name::bottom_net);
    make_post(physics, entity_id, *body, side, entity::fixture_name::top_post);
    make_post(physics, entity_id, *body, side, entity::fixture_name::bottom_post);
    make_ball_sensor(physics, entity_id, *body, vertices, entity::fixture_name::goal_line);

    return std::move(body);
}

void make_marker_sensor(
	ren::physics& physics,
	rru::ecs_db::entity_table_type::key_type entity_id,
	ren::dynamics::body& body,
	const ren::physics::vec2& position_to_body,
	entity::fixture_name fixture_name)
{
    ren::physics::circle_shape shape;
    shape.m_p.Set(position_to_body.x, position_to_body.y);
    shape.m_radius = 0.5;
    ren::contact_config<entity::collision_category> collision_config(ren::contact_result::pass_over, ren::contact_result::pass_over);
    collision_config.set(entity::collision_category::player_sensor, ren::contact_result::collide);
    ren::physics::fixture_def fixture_def = physics.make_fixture_def(
	    entity_id,
	    static_cast<std::underlying_type<entity::fixture_name>::type>(fixture_name),
	    entity::collision_category::marker_sensor,
	    collision_config);
    fixture_def.shape = &shape;
    fixture_def.density = 0;
    fixture_def.isSensor = true;
    physics.make_fixture(body, fixture_def);
}

ren::physics_ptr<ren::dynamics::body> make_center_circle(
	rru::ecs_db& db,
	float radius,
	const ren::physics::vec2& position)
{
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_staticBody;
    body_def.position.Set(position.x, position.y);
    body_def.angle = 0;
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity("center circle");
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(0, 0), entity::fixture_name::center_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(0, radius), entity::fixture_name::top_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(-radius, 0), entity::fixture_name::left_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(radius, 0), entity::fixture_name::right_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(0, -radius), entity::fixture_name::bottom_marker);
}

ren::physics_ptr<ren::dynamics::body> make_boundary(
	rru::ecs_db& db,
	float length,
	float width,
	const ren::physics::vec2& position)
{
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_staticBody;
    body_def.position.Set(position.x, position.y);
    body_def.angle = 0;
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity("boundary");
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(-480, 660), entity::fixture_name::top_left_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(0, 660), entity::fixture_name::top_center_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(480, 660), entity::fixture_name::top_right_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(-480, 20), entity::fixture_name::bottom_left_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(0, 20), entity::fixture_name::bottom_center_marker);
    make_marker_sensor(physics, entity_id, *body, ren::physics::vec2(480, 20), entity::fixture_name::bottom_right_marker);

    std::array<ren::physics::vec2, 4> top_line_vertices;
    top_line_vertices[0].Set(-482, 664);
    top_line_vertices[1].Set(482, 664);
    top_line_vertices[2].Set(482, 662);
    top_line_vertices[3].Set(-482, 662);
    make_ball_sensor(physics, entity_id, *body, top_line_vertices, entity::fixture_name::top_line);

    std::array<ren::physics::vec2, 4> bottom_line_vertices;
    bottom_line_vertices[0].Set(-482, 18);
    bottom_line_vertices[1].Set(482, 18);
    bottom_line_vertices[2].Set(482, 16);
    bottom_line_vertices[3].Set(-482, 16);
    make_ball_sensor(physics, entity_id, *body, bottom_line_vertices, entity::fixture_name::bottom_line);

    std::array<ren::physics::vec2, 4> left_top_vertices;
    left_top_vertices[0].Set(-484, 664);
    left_top_vertices[1].Set(-482, 664);
    left_top_vertices[2].Set(-482, 372);
    left_top_vertices[3].Set(-484, 372);
    make_ball_sensor(physics, entity_id, *body, left_top_vertices, entity::fixture_name::left_top_line);

    std::array<ren::physics::vec2, 4> left_bottom_vertices;
    left_bottom_vertices[0].Set(-484, 308);
    left_bottom_vertices[1].Set(-482, 308);
    left_bottom_vertices[2].Set(-482, 16);
    left_bottom_vertices[3].Set(-484, 16);
    make_ball_sensor(physics, entity_id, *body, left_bottom_vertices, entity::fixture_name::left_bottom_line);

    std::array<ren::physics::vec2, 4> right_top_vertices;
    right_top_vertices[0].Set(482, 664);
    right_top_vertices[1].Set(484, 664);
    right_top_vertices[2].Set(484, 372);
    right_top_vertices[3].Set(482, 372);
    make_ball_sensor(physics, entity_id, *body, right_top_vertices, entity::fixture_name::right_top_line);

    std::array<ren::physics::vec2, 4> right_bottom_vertices;
    right_bottom_vertices[0].Set(482, 308);
    right_bottom_vertices[1].Set(484, 308);
    right_bottom_vertices[2].Set(484, 16);
    right_bottom_vertices[3].Set(482, 16);
    make_ball_sensor(physics, entity_id, *body, right_bottom_vertices, entity::fixture_name::right_bottom_line);
}

ren::physics_ptr<ren::dynamics::body> make_penalty_box(
	rru::ecs_db& db,
	Side side,
	const ren::physics::vec2& goal_line_position)
{
    const char* entity_name = nullptr;
    std::array<ren::physics::vec2, 5> markers;
    switch (side)
    {
	case Side::LEFT:
	    entity_name = "left penalty box";
	    markers[0].Set(96.0, 0.0);
	    markers[1].Set(0.0, 160.0);
	    markers[2].Set(128.0, 160.0);
	    markers[3].Set(0.0, -160.0);
	    markers[4].Set(128.0, -160.0);
	    break;
	case Side::RIGHT:
	    entity_name = "right penalty box";
	    markers[0].Set(-96.0, 0.0);
	    markers[1].Set(-128.0, 160.0);
	    markers[2].Set(0.0, 160.0);
	    markers[3].Set(-128.0, -160.0);
	    markers[4].Set(0.0, -160.0);
	    break;
    }
    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_staticBody;
    body_def.position.Set(goal_line_position.x, goal_line_position.y);
    body_def.angle = 0;
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity(entity_name);
    ren::physics_ptr<ren::dynamics::body> body = std::move(physics.make_body(entity_id, body_def));

    make_marker_sensor(physics, entity_id, *body, markers[0], entity::fixture_name::center_marker);
    make_marker_sensor(physics, entity_id, *body, markers[1], entity::fixture_name::top_left_marker);
    make_marker_sensor(physics, entity_id, *body, markers[2], entity::fixture_name::top_right_marker);
    make_marker_sensor(physics, entity_id, *body, markers[3], entity::fixture_name::bottom_left_marker);
    make_marker_sensor(physics, entity_id, *body, markers[4], entity::fixture_name::bottom_right_marker);

    return std::move(body);
}

} // anonymous namespace

field make_field(rru::ecs_db& db)
{
    field result{
	    make_goal(db, Side::LEFT, ren::physics::vec2(-480, 340)),
	    make_goal(db, Side::RIGHT, ren::physics::vec2(480, 340)),
	    make_center_circle(db, 80, ren::physics::vec2(0, 340)),
	    make_boundary(db, 480 * 2, 340 * 2, ren::physics::vec2(0, 0)),
	    make_penalty_box(db, Side::LEFT, ren::physics::vec2(-480, 340)),
	    make_penalty_box(db, Side::RIGHT, ren::physics::vec2(480, 340))};
    return std::move(result);
}

} // namespace common
} // namespace robocup2Dsim

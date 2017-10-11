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
    std::array<b2Vec2, 2> vertices;
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

ren::physics_ptr<ren::dynamics::body> make_goal(rru::ecs_db& db, Side side, const ren::physics::vec2& position)
{
    const char* entity_name = nullptr;
    switch (side)
    {
	case Side::LEFT:
	    entity_name = "left goal";
	    break;
	case Side::RIGHT:
	    entity_name = "right goal";
	    break;
    }

    ren::physics& physics = ren::update_physics_instance(db);
    ren::physics::body_def body_def;
    body_def.type = ren::physics::body_type::b2_staticBody;
    body_def.position.Set(position.x, position.y);
    body_def.angle = 0;
    rru::ecs_db::entity_table_type::key_type entity_id = db.insert_entity(entity_name);
    ren::physics_ptr<ren::dynamics::body> goal_body = std::move(physics.make_body(entity_id, body_def));

    make_back_net(physics, entity_id, *goal_body, side, entity::fixture_name::back_net);
    make_side_net(physics, entity_id, *goal_body, side, entity::fixture_name::top_net);
    make_side_net(physics, entity_id, *goal_body, side, entity::fixture_name::bottom_net);

    return std::move(goal_body);
}

} // anonymous namespace

field make_field(rru::ecs_db& db)
{
    field result{
	    make_goal(db, Side::LEFT, ren::physics::vec2(-480, 340)),
	    make_goal(db, Side::RIGHT, ren::physics::vec2(480, 340))};
    return std::move(result);
}

} // namespace common
} // namespace robocup2Dsim

#include "physics.hpp"
#include "physics.hxx"
#include <cstdint>
#include <type_traits>
#include <utility>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <robocup2Dsim/runtime/ecs_db.hxx>
#include <robocup2Dsim/runtime/ram_db.hxx>
#include <robocup2Dsim/runtime/resource.hxx>
#include "dynamics.hpp"

namespace rru = robocup2Dsim::runtime;

namespace {

using namespace robocup2Dsim::engine;

void delete_body(dynamics::body* body)
{
     physics* engine = static_cast<physics*>(body->GetUserData());
     if (engine != nullptr)
     {
	 engine->destroy_body(body);
     }
}

} // anonymous namespace

namespace robocup2Dsim {
namespace engine {

physics::physics()
    :
	physics(vec2(0.0, 0.0), {8U, 4U})
{ }

physics::physics(const vec2& gravity, const solver_config& solver_conf)
    :
	world_(gravity),
	solver_conf_{solver_conf.velocity_iteration_limit, solver_conf.position_iteration_limit}
{ }

physics_ptr<dynamics::body> physics::make_body(entity_id_type, const body_def& def)
{
    physics_ptr<dynamics::body> body(world_.CreateBody(&def), &delete_body);
    body->SetUserData(reinterpret_cast<void*>(this));
    return std::move(body);
}

void physics::destroy_body(dynamics::body* body)
{
    world_.DestroyBody(body);
}

void physics::make_fixture(
	dynamics::body& body,
	const fixture_def& def)
{
    fixture* result = body.CreateFixture(&def);
}

void physics::make_joint(
	entity_id_type entity_id,
	const revolute_joint_def& def)
{
    make_joint<revolute_joint>(entity_id, def);
}

void physics::make_joint(
	entity_id_type entity_id,
	const prismatic_joint_def& def)
{
    make_joint<prismatic_joint>(entity_id, def);
}

void register_system(rru::ecs_db& db, std::unique_ptr<physics> phys)
{
    rru::ecs_db::system_table_type& sys_table = db.access<ecs_db::system_table_type>(table_id::system_registry);
    physics_table_type* table = new physics_table_type(1U, "task_id", "physics_instance", "instance_name");
    table->emplace(0U, std::move(phys), "active_box2d");
    sys_table.emplace(system_id::physics, make_unique_table(table), "physics");

    rru::ecs_db::component_system_table_type& comp_sys_map = db.access<ecs_db::component_system_table_type>(
	    table_id::component_system_map);
    comp_sys_map.auto_emplace(component_id::body, system_id::physics);
}

} // namespace engine
} // robocup2Dsim

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

namespace robocup2Dsim {
namespace engine {

namespace {

using namespace robocup2Dsim::engine;

static constexpr rru::primitives::key_16 default_physics_instance_id = 0U;

void delete_body(dynamics::body* body)
{
     physics* engine = static_cast<physics*>(body->GetUserData());
     if (engine != nullptr)
     {
	 engine->destroy_body(body);
     }
}

} // anonymous namespace

physics::physics()
    :
	physics(vec2(0.0, 0.0), {8U, 4U})
{ }

physics::physics(const vec2& gravity, const solver_config& solver_conf)
    :
	world_(gravity),
	solver_conf_{solver_conf.velocity_iteration_limit, solver_conf.position_iteration_limit}
{ }

physics_ptr<dynamics::body> physics::make_body(rru::ecs_db::entity_id_type, const body_def& def)
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
    body.CreateFixture(&def);
}

void physics::make_joint(const revolute_joint_def& def)
{
    make_joint<revolute_joint>(def);
}

void physics::make_joint(const prismatic_joint_def& def)
{
    make_joint<prismatic_joint>(def);
}

void register_system(rru::ecs_db& db, std::unique_ptr<physics> phys)
{
    rru::ecs_db::system_table_type& sys_table = db.access<rru::ecs_db::system_table_type>(rru::table_id::system_registry);
    physics_table_type* table = new physics_table_type(default_physics_instance_id, "task_id", "physics_instance", "instance_name");
    table->emplace(0U, std::move(phys), "active_box2d");
    sys_table.emplace(rru::system_id::physics, make_unique_table(table), "physics");

    rru::ecs_db::component_system_table_type& comp_sys_map = db.access<rru::ecs_db::component_system_table_type>(
	    rru::table_id::component_system_map);
    comp_sys_map.auto_emplace(rru::component_id::body, rru::system_id::physics);
}

const physics& select_physics_instance(const robocup2Dsim::runtime::ecs_db& db)
{
    const rru::ecs_db::system_table_type& sys_table = db.access<rru::ecs_db::system_table_type>(rru::table_id::system_registry);
    auto iter = sys_table.select_row(rru::system_id::physics);
    if (iter == sys_table.cend())
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    return *(iter.select_column<std::unique_ptr<physics>>("physics_instance"));
}

physics& update_physics_instance(robocup2Dsim::runtime::ecs_db& db)
{
    rru::ecs_db::system_table_type& sys_table = db.access<rru::ecs_db::system_table_type>(rru::table_id::system_registry);
    auto iter = sys_table.update_row(rru::system_id::physics);
    if (iter == sys_table.end())
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    return *(iter.update_column<std::unique_ptr<physics>>("physics_instance"));
}

} // namespace engine
} // robocup2Dsim

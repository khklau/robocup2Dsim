#include "physics.hpp"
#include "physics.hh"
#include <cstdint>
#include <type_traits>
#include <utility>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <robocup2Dsim/runtime/db_access.hpp>
#include <robocup2Dsim/runtime/ecs_db.hh>
#include <robocup2Dsim/runtime/ram_db.hh>
#include <robocup2Dsim/runtime/resource.hh>
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

physics_ptr<dynamics::body> physics::make_body(const rru::ecs_db::entity_id_type, const body_def& def)
{
    physics_ptr<dynamics::body> body(world_.CreateBody(&def), &delete_body);
    body->SetUserData(reinterpret_cast<void*>(this));
    return std::move(body);
}

void physics::make_body(
        const rru::ecs_db::entity_id_type,
        const body_def& def,
        dynamics::body& place)
{
    world_.CreateBody(&def, &place);
    place.SetUserData(reinterpret_cast<void*>(this));
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

void physics::apply_linear_impulse(dynamics::body& body, const vec2& impulse) const
{
    body.ApplyLinearImpulse(impulse, body.GetWorldCenter(), true);
}

void physics::apply_angular_impulse(dynamics::body& body, float impulse) const
{
    body.ApplyAngularImpulse(impulse, true);
}

std::unique_ptr<physics> register_system(rru::ecs_db& db, std::unique_ptr<physics> phys)
{
    rru::ecs_db::system_table_type& sys_table = db.access<rru::ecs_db::system_table_type>(rru::table_id::system_registry);
    auto sys_iter = sys_table.update_row(rru::system_id::physics);
    if (sys_iter != sys_table.end())
    {
        physics_table_type* table = static_cast<physics_table_type*>(static_cast<void*>(std::get<1>(*sys_iter).get()));
        auto phys_iter = table->update_row(default_physics_instance_id);
        if (phys_iter != table->end())
        {
            phys_iter.update_column<std::unique_ptr<physics>>("physics_instance").swap(phys);
        }
        else
        {
            table->emplace(default_physics_instance_id, std::move(phys), "active_box2d");
        }
    }
    else
    {
        physics_table_type* table = new physics_table_type(1U, "task_id", "physics_instance", "instance_name");
        table->emplace(default_physics_instance_id, std::move(phys), "active_box2d");
        sys_table.emplace(rru::system_id::physics, make_unique_table(table), "physics");
    }

    rru::ecs_db::component_system_table_type& comp_sys_map = db.access<rru::ecs_db::component_system_table_type>(
	    rru::table_id::component_system_map);
    comp_sys_map.auto_emplace(rru::component_id::body, rru::system_id::physics);
    return std::move(phys);
}

const physics& select_physics_instance(const robocup2Dsim::runtime::ecs_db& db)
{
    const rru::ecs_db::system_table_type& sys_table = db.access<rru::ecs_db::system_table_type>(rru::table_id::system_registry);
    auto sys_iter = sys_table.select_row(rru::system_id::physics);
    if (sys_iter == sys_table.cend())
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    const physics_table_type* table = static_cast<const physics_table_type*>(static_cast<void*>(std::get<1>(*sys_iter).get()));
    if (table == nullptr)
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    auto phys_iter = table->select_row(default_physics_instance_id);
    if (phys_iter == table->cend())
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    return *(phys_iter.select_column<std::unique_ptr<physics>>("physics_instance"));
}

physics& update_physics_instance(robocup2Dsim::runtime::ecs_db& db)
{
    rru::ecs_db::system_table_type& sys_table = db.access<rru::ecs_db::system_table_type>(rru::table_id::system_registry);
    auto sys_iter = sys_table.update_row(rru::system_id::physics);
    if (sys_iter == sys_table.end())
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    physics_table_type* table = static_cast<physics_table_type*>(static_cast<void*>(std::get<1>(*sys_iter).get()));
    if (table == nullptr)
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    auto phys_iter = table->update_row(default_physics_instance_id);
    if (phys_iter == table->end())
    {
	throw rru::invalid_deference_error("physics instance not registered");
    }
    return *(phys_iter.update_column<std::unique_ptr<physics>>("physics_instance"));
}

} // namespace engine
} // robocup2Dsim

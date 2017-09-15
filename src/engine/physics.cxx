#include "physics.hpp"
#include "physics.hxx"
#include <cstdint>
#include <type_traits>
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

physics::physics()
    :
	physics(vec2(0.0, 0.0))
{ }

physics::physics(const vec2& gravity)
    :
	world_(gravity)
{ }

physics_ptr<dynamics::body> physics::make_body(entity_id_type entity_id, const body_def& def)
{
    physics_ptr<dynamics::body> body(world_.CreateBody(&def));
    body->SetUserData(reinterpret_cast<void*>(static_cast<std::uintptr_t>(entity_id)));
    return body;
}

void physics::make_fixture(
	entity_id_type entity_id,
	physics_ptr<dynamics::body> body,
	const fixture_def& def)
{
    physics_ptr<fixture> result(body->CreateFixture(&def));
    result->SetUserData(reinterpret_cast<void*>(static_cast<std::uintptr_t>(entity_id)));
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

#include "physics.hpp"
#include <Box2D/Common/b2Math.h>
#include <SnapBox2D/snapshot.hpp>
#include <SnapBox2D/storage.hpp>
#include "ecs_db.hpp"
#include "ecs_db.hxx"
#include "ram_db.hxx"

namespace robocup2Dsim {
namespace engine {

physics::physics()
    :
	world_(b2Vec2(0, 0))
{ }

void register_system(ecs_db& db, std::unique_ptr<physics> phys)
{
    ecs_db::system_table_type& sys_table = db.access<ecs_db::system_table_type>(table_id::system_registry);
    physics_table_type* table = new physics_table_type(1U, "task_id", "physics_instance", "instance_name");
    table->emplace(0U, std::move(phys), "active_box2d");
    sys_table.emplace(system_id::physics, make_unique_table(table), "physics");
}

} // namespace engine
} // robocup2Dsim

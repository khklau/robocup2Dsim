#include "physics.hpp"
#include <Box2D/Common/b2Math.h>
#include <robocup2Dsim/runtime/ecs_db.hpp>
#include <robocup2Dsim/runtime/ecs_db.hxx>
#include <robocup2Dsim/runtime/ram_db.hxx>
#include "dynamics.hpp"

namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace engine {

physics::physics()
    :
	world_(b2Vec2(0, 0))
{ }

void register_system(rru::ecs_db& db, std::unique_ptr<physics> phys)
{
    rru::ecs_db::system_table_type& sys_table = db.access<ecs_db::system_table_type>(table_id::system_registry);
    physics_table_type* table = new physics_table_type(1U, "task_id", "physics_instance", "instance_name");
    table->emplace(0U, std::move(phys), "active_box2d");
    sys_table.emplace(system_id::physics, make_unique_table(table), "physics");
}

} // namespace engine
} // robocup2Dsim

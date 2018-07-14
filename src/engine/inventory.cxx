#include "inventory.hpp"
#include <robocup2Dsim/runtime/primitives.hpp>
#include <robocup2Dsim/runtime/ecs_db.hh>

namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace engine {

namespace {

static constexpr rru::primitives::key_16 default_inventory_instance_id = 0U;

} // anonyous namespace

inventory::inventory(const inventory_policy& policy)
    :
	policy_(policy)
{ }

void register_system(rru::ecs_db& db, std::unique_ptr<inventory> inv)
{
    rru::ecs_db::system_table_type& sys_table = db.access<rru::ecs_db::system_table_type>(rru::table_id::system_registry);
    inventory_table_type* table = new inventory_table_type(default_inventory_instance_id, "task_id", "inventory_instance", "instance_name");
    table->emplace(0U, std::move(inv), "active_inventory");
    sys_table.emplace(rru::system_id::inventory, make_unique_table(table), "inventory");

    rru::ecs_db::component_system_table_type& comp_sys_map = db.access<rru::ecs_db::component_system_table_type>(
	    rru::table_id::component_system_map);
    comp_sys_map.auto_emplace(rru::component_id::body, rru::system_id::physics);
}

} // namespace engine
} // namespace robocup2Dsim

#include "ecs_db.hpp"
#include "ram_db.hxx"

namespace robocup2Dsim {
namespace engine {

ecs_db::ecs_db(std::size_t contingency_capacity, const std::vector<turbo::memory::block_config>& allocator_config)
    :
	component_allocator_(contingency_capacity, allocator_config),
	catalog_(64U, "table_id", "table_name", "table_ptr")
{
    catalog_.emplace(table_id::entity_registry, "entity_registry", make_unique_table(
	    new entity_table_type(32U, "entity_id", "entity_name")));
    catalog_.emplace(table_id::component_registry, "component_registry", make_unique_table(
	    new component_table_type(64U, "component_id", "component_name")));
    catalog_.emplace(table_id::system_registry, "system_registry", make_unique_table(
	    new system_table_type(8U, "system_id", "system_table_ptr", "system_name")));
    catalog_.emplace(table_id::entity_component_map, "entity_component_map", make_unique_table(
	    new entity_component_table_type(2048U, "entity_id", "component_id", "value_id")));
    catalog_.emplace(table_id::component_system_map, "component_system_map", make_unique_table(
	    new component_system_table_type(512U, "component_id", "system_id", "task_id")));
}

} // namespace engine
} // namespace robocup2Dsim

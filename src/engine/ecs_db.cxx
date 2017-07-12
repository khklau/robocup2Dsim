#include "ecs_db.hpp"
#include "ram_db.hxx"

namespace ren = robocup2Dsim::engine;

namespace robocup2Dsim {
namespace engine {

ecs_db::ecs_db()
    :
	catalog_(64U, "table_id", "table_name", "table_ptr")
{
    catalog_.emplace(table_id::entity, "entity", ren::make_unique_table(
	    new entity_table_type(32U, "entity_id", "entity_name")));
    catalog_.emplace(table_id::component, "component", ren::make_unique_table(
	    new component_table_type(512U, "component_id", "component_name")));
    catalog_.emplace(table_id::system, "system", ren::make_unique_table(
	    new system_table_type(8U, "system_id", "system_name")));
}

} // namespace engine
} // namespace robocup2Dsim

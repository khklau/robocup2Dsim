#include "ecs_db.hpp"
#include <robocup2Dsim/engine/ram_db.hxx>

namespace ren = robocup2Dsim::engine;

namespace robocup2Dsim {
namespace common {

ecs_db::ecs_db()
    :
	catalog_(64U, "table_id", "table_name", "table_ptr")
{
    catalog_.emplace(0U, "entity", ren::make_unique_table(
	    new table<ren::key_16, ren::fixed_cstring_32>(256U, "entity_id", "entity_name")));
}

} // namespace common
} // namespace robocup2Dsim

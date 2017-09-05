#include "dynamics.hpp"
#include <robocup2Dsim/runtime/ecs_db.hxx>

namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace engine {
namespace dynamics {

void register_components(robocup2Dsim::runtime::ecs_db& db)
{
    rru::ecs_db::component_table_type& comp_table = db.access<ecs_db::component_table_type>(table_id::component_registry);
    comp_table.emplace(component_id::body, "body");
}

} // namespace dynamics
} // namespace engine
} // namespace robocup2Dsim

#ifndef ROBOCUP2DSIM_ENGINE_ECS_DB_HXX
#define ROBOCUP2DSIM_ENGINE_ECS_DB_HXX

#include <robocup2Dsim/engine/ram_db.hxx>

namespace robocup2Dsim {
namespace engine {

template <class table_t>
table_t& ecs_db::access(table_id::type id)
{
    auto iter = catalog_.update_row(id);
    return *(static_cast<table_t*>(static_cast<void*>(iter.set_column<unique_table_ptr>("table_ptr").get())));
}

template <class table_t>
const table_t& ecs_db::access(table_id::type id) const
{
    auto iter = catalog_.select_row(id);
    return *(static_cast<table_t*>(static_cast<void*>(iter.get_column<unique_table_ptr>("table_ptr").get())));
}

} // namespace engine
} // namespace robocup2Dsim

#endif

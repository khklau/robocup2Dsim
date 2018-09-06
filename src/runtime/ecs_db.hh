#ifndef ROBOCUP2DSIM_RUNTIME_ECS_DB_HXX
#define ROBOCUP2DSIM_RUNTIME_ECS_DB_HXX

#include <robocup2Dsim/runtime/ecs_db.hpp>
#include <robocup2Dsim/runtime/ram_db.hh>

namespace robocup2Dsim {
namespace runtime {

template <class table_t>
table_t& ecs_db::access(table_id::type id)
{
    auto iter = catalog_.update_row(id);
    return *(static_cast<table_t*>(static_cast<void*>(iter.update_column<unique_table_ptr>("table_ptr").get())));
}

template <class table_t>
const table_t& ecs_db::access(table_id::type id) const
{
    auto iter = catalog_.select_row(id);
    return *(static_cast<table_t*>(static_cast<void*>(iter.select_column<unique_table_ptr>("table_ptr").get())));
}

} // namespace runtime
} // namespace robocup2Dsim

#endif

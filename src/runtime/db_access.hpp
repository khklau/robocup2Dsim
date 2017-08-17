#ifndef ROBOCUP2DSIM_RUNTIME_DB_ACCESS_HPP
#define ROBOCUP2DSIM_RUNTIME_DB_ACCESS_HPP

#include <turbo/memory/slab_allocator.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {

namespace runtime {

class ecs_db;

} // namespace runtime

namespace runtime {

robocup2Dsim::runtime::ecs_db& update_local_db();

inline const robocup2Dsim::runtime::ecs_db& select_local_db()
{
    return update_local_db();
}

} // namespace runtime

} // namespace robocup2Dsim

namespace runtime_ {

turbo::memory::concurrent_sized_slab& local_allocator();

} // namespace runtime_

#endif

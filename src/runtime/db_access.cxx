#include "db_access.hpp"
#include <turbo/memory/slab_allocator.hxx>
#include <robocup2Dsim/runtime/ecs_db.hxx>

namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace runtime {

rru::ecs_db& update_local_db()
{
    thread_local rru::ecs_db* db_ = nullptr;
    if (!db_)
    {
	// TODO: parse the component allocator config from a file
	db_ = new rru::ecs_db(4U,
	{
	    {2U, 256U},
	    {4U, 256U},
	    {8U, 256U},
	    {16U, 128U},
	    {32U, 128U},
	    {64U, 128U},
	    {128U, 64U},
	    {256U, 32U},
	    {512U, 16U},
	    {1024U, 8U},
	    {2048U, 4U},
	});
    }
    return *db_;
}

} // namespace runtime
} // namespace robocup2Dsim

namespace runtime_ {

turbo::memory::concurrent_sized_slab& local_allocator()
{
    return robocup2Dsim::runtime::update_local_db().component_allocator();
}

} // namespace runtime_

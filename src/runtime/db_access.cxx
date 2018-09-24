#include "db_access.hpp"
#include <turbo/memory/slab_allocator.hh>
#include <robocup2Dsim/runtime/ecs_db.hh>

namespace robocup2Dsim {
namespace runtime {

ecs_db& update_local_db()
{
    thread_local ecs_db* db_ = nullptr;
    if (db_ == nullptr)
    {
	// TODO: parse the component allocator config from a file
	db_ = new ecs_db(4U,
	{
	    {2U, 256U},
	    {4U, 256U},
	    {8U, 256U},
	    {16U, 128U},
	    {32U, 128U},
	    {64U, 128U},
	    {128U, 64U},
	    {256U, 64U},
	    {512U, 32U},
	    {1024U, 32U},
	    {1280U, 16U},
	    {2048U, 8U},
	    {4096U, 4U},
	    {8192U, 2U},
	    {16384U, 2U},
	    {20480U, 2U},
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

#include <robocup2Dsim/runtime/db_access.hpp>
#include <cstddef>
#include <gtest/gtest.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>

TEST(db_access_test, local_allocator_basic)
{
    b2World world1(b2Vec2(0, 0));
    world1.SetGravity(b2Vec2(0, 0));
    std::size_t* tmp1 = runtime_::local_allocator().allocate<std::size_t>();
    EXPECT_NE(nullptr, tmp1);
    runtime_::local_allocator().deallocate<std::size_t>(tmp1);
}

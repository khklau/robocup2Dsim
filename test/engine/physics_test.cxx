#include <robocup2Dsim/engine/physics.hpp>
#include <cstddef>
#include <memory>
#include <utility>
#include <gtest/gtest.h>
#include <robocup2Dsim/runtime/db_access.hpp>

namespace ren = robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;

TEST(db_access_test, local_allocator_basic)
{
    std::unique_ptr<ren::physics> physics1(new ren::physics());
    ren::register_system(rru::update_local_db(), std::move(physics1));
}


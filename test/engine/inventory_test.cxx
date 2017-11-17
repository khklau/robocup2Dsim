#include <robocup2Dsim/engine/inventory.hpp>
#include <robocup2Dsim/engine/inventory.hxx>
#include <gtest/gtest.h>

namespace ren = robocup2Dsim::engine;

TEST(inventory_test, make_energy_basic)
{
    ren::energy value1{100U};
    auto array1 = ren::make_energy<4U>(value1);
    EXPECT_EQ(4U, array1->size())
	    << "make_energy did not create an array of the correct length";
    EXPECT_EQ(4U, std::count(array1->cbegin(), array1->cend(), value1))
	    << "make_energy did not initialise the array wit the correct value";
}

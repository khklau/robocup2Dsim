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

TEST(inventory_test, accrue_invalid)
{
    ren::energy value1{100U};
    auto stock1 = ren::make_energy<4U>(value1);
    ren::inventory_policy policy1{0U, 100U};
    ren::inventory inventory1(policy1);
    std::array<ren::energy, 4U> amount1{1U, 2U, 5U, 10U};
    auto result1 = inventory1.accrue(*stock1, amount1);
    for (const ren::inventory::accrue_result result: result1)
    {
	EXPECT_EQ(ren::inventory::accrue_result::oversupply, result) << "Accruing to a full inventory succeeded";
    }
}

TEST(inventory_test, spend_invalid)
{
    ren::energy value1{0U};
    auto stock1 = ren::make_energy<4U>(value1);
    ren::inventory_policy policy1{0U, 100U};
    ren::inventory inventory1(policy1);
    std::array<ren::energy, 4U> amount1{1U, 2U, 5U, 10U};
    auto result1 = inventory1.spend(*stock1, amount1);
    for (const ren::inventory::spend_result result: result1)
    {
	EXPECT_EQ(ren::inventory::spend_result::understock, result) << "Spending an empty inventory succeeded";
    }
}

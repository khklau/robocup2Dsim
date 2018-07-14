#include <robocup2Dsim/engine/inventory.hpp>
#include <robocup2Dsim/engine/inventory.hh>
#include <gtest/gtest.h>
#include <limits>

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

    ren::energy value2{50U};
    auto stock2 = ren::make_energy<4U>(value2);
    ren::inventory_policy policy2{0U, 100U};
    ren::inventory inventory2(policy2);
    std::array<ren::energy, 4U> amount2{100U, 20U, 5U, 51U};
    auto result2 = inventory2.accrue(*stock2, amount2);
    EXPECT_EQ(ren::inventory::accrue_result::oversupply, result2[0]) << "Accruing over the maximum succeeded";
    EXPECT_EQ(ren::inventory::accrue_result::success, result2[1]) << "Accruing below the maximum failed";
    EXPECT_EQ(ren::inventory::accrue_result::success, result2[2]) << "Accruing below the maximum failed";
    EXPECT_EQ(ren::inventory::accrue_result::oversupply, result2[3]) << "Accruing over the maximum succeeded";
}

TEST(inventory_test, accrue_overflow)
{
    ren::energy value1{std::numeric_limits<decltype(ren::energy::quantity)>::max()};
    auto stock1 = ren::make_energy<4U>(value1);
    ren::inventory_policy policy1{0U, std::numeric_limits<decltype(ren::energy::quantity)>::max()};
    ren::inventory inventory1(policy1);
    std::array<ren::energy, 4U> amount1{1U, 2U, 5U, 10U};
    auto result1 = inventory1.accrue(*stock1, amount1);
    for (const ren::inventory::accrue_result result: result1)
    {
	EXPECT_EQ(ren::inventory::accrue_result::oversupply, result) << "Accruing to a full inventory succeeded";
    }
}

TEST(inventory_test, accrue_basic)
{
    ren::energy value1{90U};
    auto stock1 = ren::make_energy<4U>(value1);
    ren::inventory_policy policy1{0U, 100U};
    ren::inventory inventory1(policy1);
    std::array<ren::energy, 4U> amount1{1U, 2U, 5U, 10U};
    auto result1 = inventory1.accrue(*stock1, amount1);
    for (const ren::inventory::accrue_result result: result1)
    {
	EXPECT_EQ(ren::inventory::accrue_result::success, result) << "Accruing under the maximum failed";
    }
    EXPECT_EQ(90U + 1U, (*stock1)[0].quantity) << "Accruing with specified amount failed";
    EXPECT_EQ(90U + 2U, (*stock1)[1].quantity) << "Accruing with specified amount failed";
    EXPECT_EQ(90U + 5U, (*stock1)[2].quantity) << "Accruing with specified amount failed";
    EXPECT_EQ(90U + 10U, (*stock1)[3].quantity) << "Accruing with specified amount failed";

    std::array<ren::energy, 4U> stock2{90U, 40U, 0U, 80U};
    ren::inventory_policy policy2{0U, 100U};
    ren::inventory inventory2(policy2);
    std::array<ren::energy, 4U> amount2{10U, 10U, 10U, 10U};
    auto result2 = inventory2.accrue(stock2, amount2);
    for (const ren::inventory::accrue_result result: result2)
    {
	EXPECT_EQ(ren::inventory::accrue_result::success, result) << "Accruing under the maximum failed";
    }
    EXPECT_EQ(90U + 10U, stock2[0].quantity) << "Accruing with specified amount failed";
    EXPECT_EQ(40U + 10U, stock2[1].quantity) << "Accruing with specified amount failed";
    EXPECT_EQ(0U + 10U, stock2[2].quantity) << "Accruing with specified amount failed";
    EXPECT_EQ(80U + 10U, stock2[3].quantity) << "Accruing with specified amount failed";
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

    ren::energy value2{10U};
    auto stock2 = ren::make_energy<4U>(value2);
    ren::inventory_policy policy2{0U, 100U};
    ren::inventory inventory2(policy2);
    std::array<ren::energy, 4U> amount2{11U, 2U, 5U, 40U};
    auto result2 = inventory2.spend(*stock2, amount2);
    EXPECT_EQ(ren::inventory::spend_result::understock, result2[0]) << "Spending under the minumum succeeded";
    EXPECT_EQ(ren::inventory::spend_result::success, result2[1]) << "Spending above the minimum failed";
    EXPECT_EQ(ren::inventory::spend_result::success, result2[2]) << "Spending above the minimum failed";
    EXPECT_EQ(ren::inventory::spend_result::understock, result2[3]) << "Spending under the minimum succeeded";
}

TEST(inventory_test, spend_underflow)
{
    ren::energy value1{std::numeric_limits<decltype(ren::energy::quantity)>::min()};
    auto stock1 = ren::make_energy<4U>(value1);
    ren::inventory_policy policy1{std::numeric_limits<decltype(ren::energy::quantity)>::min(), 100U};
    ren::inventory inventory1(policy1);
    std::array<ren::energy, 4U> amount1{1U, 2U, 5U, 10U};
    auto result1 = inventory1.spend(*stock1, amount1);
    for (const ren::inventory::spend_result result: result1)
    {
	EXPECT_EQ(ren::inventory::spend_result::understock, result) << "Spending an empty inventory succeeded";
    }
}

TEST(inventory_test, spend_basic)
{
    ren::energy value1{20U};
    auto stock1 = ren::make_energy<4U>(value1);
    ren::inventory_policy policy1{0U, 100U};
    ren::inventory inventory1(policy1);
    std::array<ren::energy, 4U> amount1{1U, 8U, 15U, 20U};
    auto result1 = inventory1.spend(*stock1, amount1);
    for (const ren::inventory::spend_result result: result1)
    {
	EXPECT_EQ(ren::inventory::spend_result::success, result) << "Spending above the minimum failed";
    }
    EXPECT_EQ(20U - 1U, (*stock1)[0].quantity) << "Spending with specified amount failed";
    EXPECT_EQ(20U - 8U, (*stock1)[1].quantity) << "Spending with specified amount failed";
    EXPECT_EQ(20U - 15U, (*stock1)[2].quantity) << "Spending with specified amount failed";
    EXPECT_EQ(20U - 20U, (*stock1)[3].quantity) << "Spending with specified amount failed";

    std::array<ren::energy, 4U> stock2{10U, 0U, 20U, 0U};
    ren::inventory_policy policy2{0U, 100U};
    ren::inventory inventory2(policy2);
    std::array<ren::energy, 4U> amount2{10U, 0U, 20U, 0U};
    auto result2 = inventory2.spend(stock2, amount2);
    for (const ren::inventory::spend_result result: result2)
    {
	EXPECT_EQ(ren::inventory::spend_result::success, result) << "Spending under the minimum failed";
    }
    EXPECT_EQ(10U - 10U, stock2[0].quantity) << "Spending with specified amount failed";
    EXPECT_EQ(0U - 0U, stock2[1].quantity) << "Spending with specified amount failed";
    EXPECT_EQ(20U - 20U, stock2[2].quantity) << "Spending with specified amount failed";
    EXPECT_EQ(0U - 0U, stock2[3].quantity) << "Spending with specified amount failed";
}

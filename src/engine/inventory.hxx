#ifndef ROBOCUP2DSIM_ENGINE_INVENTORY_HXX
#define ROBOCUP2DSIM_ENGINE_INVENTORY_HXX

#include <algorithm>
#include <turbo/memory/slab_allocator.hpp>
#include <turbo/memory/slab_allocator.hxx>
#include <turbo/toolset/extension.hpp>
#include <robocup2Dsim/runtime/db_access.hpp>

namespace robocup2Dsim {
namespace engine {

template <std::size_t length_c>
turbo::memory::slab_unique_ptr<std::array<energy, length_c>> make_energy(energy initial_value)
{
    auto result = runtime_::local_allocator().make_unique<std::array<energy, length_c>>();
    if (TURBO_LIKELY(result.first == turbo::memory::make_result::success))
    {
	std::fill_n(result.second->begin(), length_c, initial_value);
	return std::move(result.second);
    }
    else
    {
	throw turbo::memory::out_of_memory_error("Insufficient free memory to allocate the requested energy array");
    }
}

template <class item_t>
inventory::accrue_result inventory::accrue(item_t& stock, const item_t& amount) const
{
    // Need to be careful of potential over flow
    if (amount.quantity <= policy_.maximum.quantity - stock.quantity)
    {
	stock.quantity += amount.quantity;
	return accrue_result::success;
    }
    else
    {
	return accrue_result::oversupply;
    }
}

template <class item_t>
inventory::spend_result inventory::spend(item_t& stock, const item_t& amount) const
{
    // Need to be careful of potential under flow
    if (amount.quantity <= stock.quantity - policy_.minimum.quantity)
    {
	stock.quantity -= amount.quantity;
	return spend_result::success;
    }
    else
    {
	return spend_result::understock;
    }
}

template <class item_t, std::size_t length_c>
std::array<inventory::accrue_result, length_c> inventory::accrue(
	std::array<item_t, length_c>& stock,
	const std::array<item_t, length_c>& amount) const
{
    std::array<accrue_result, length_c> result;
    auto stock_iter = stock.begin();
    auto amount_iter = amount.cbegin();
    auto result_iter = result.begin();
    for (; stock_iter != stock.end() && amount_iter != amount.cend() && result_iter != result.end();
	    ++stock_iter, ++amount_iter, ++result_iter)
    {
	*result_iter = accrue(*stock_iter, *amount_iter);
    }
    return std::move(result);
}

template <class item_t, std::size_t length_c>
std::array<inventory::spend_result, length_c> inventory::spend(
	std::array<item_t, length_c>& stock,
	const std::array<item_t, length_c>& amount) const
{
    std::array<spend_result, length_c> result;
    auto stock_iter = stock.begin();
    auto amount_iter = amount.cbegin();
    auto result_iter = result.begin();
    for (; stock_iter != stock.end() && amount_iter != amount.cend() && result_iter != result.end();
	    ++stock_iter, ++amount_iter, ++result_iter)
    {
	*result_iter = spend(*stock_iter, *amount_iter);
    }
    return std::move(result);
}

} // namespace engine
} // namespace robocup2Dsim

#endif

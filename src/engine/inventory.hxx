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

template <class item_t, std::size_t length_c>
std::array<inventory::accrue_result, length_c> inventory::accrue(
	std::array<item_t, length_c>& stock,
	const std::array<item_t, length_c>& amount)
{
    std::array<accrue_result, length_c> result;
    std::fill_n(result.begin(), result.max_size(), accrue_result::success);
    auto stock_iter = stock.begin();
    auto amount_iter = amount.cbegin();
    auto result_iter = result.begin();
    for (; stock_iter != stock.end() && amount_iter != amount.cend() && result_iter != result.end();
	    ++stock_iter, ++amount_iter, ++result_iter)
    {
	// Need to be careful of potential over flow
	if (amount_iter->quantity <= policy_.maximum.quantity - stock_iter->quantity)
	{
	    stock_iter->quantity += amount_iter->quantity;
	}
	else
	{
	    *result_iter = accrue_result::oversupply;
	}
    }
    return std::move(result);
}

template <class item_t, std::size_t length_c>
std::array<inventory::spend_result, length_c> inventory::spend(
	std::array<item_t, length_c>& stock,
	const std::array<item_t, length_c>& amount)
{
    std::array<spend_result, length_c> result;
    std::fill_n(result.begin(), result.max_size(), spend_result::success);
    auto stock_iter = stock.begin();
    auto amount_iter = amount.cbegin();
    auto result_iter = result.begin();
    for (; stock_iter != stock.end() && amount_iter != amount.cend() && result_iter != result.end();
	    ++stock_iter, ++amount_iter, ++result_iter)
    {
	// Need to be careful of potential under flow
	if (amount_iter->quantity <= stock_iter->quantity - policy_.minimum.quantity)
	{
	    stock_iter->quantity -= amount_iter->quantity;
	}
	else
	{
	    *result_iter = spend_result::understock;
	}
    }
    return std::move(result);
}

} // namespace engine
} // namespace robocup2Dsim

#endif

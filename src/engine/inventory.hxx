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

} // namespace engine
} // namespace robocup2Dsim

#endif

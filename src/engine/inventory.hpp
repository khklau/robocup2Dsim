#ifndef ROBOCUP2DSIM_ENGINE_INVENTORY_HPP
#define ROBOCUP2DSIM_ENGINE_INVENTORY_HPP

#include <cstdint>
#include <array>
#include <turbo/memory/slab_allocator.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>
#include <robocup2Dsim/runtime/ram_db.hpp>

namespace robocup2Dsim {
namespace engine {

struct TURBO_SYMBOL_DECL energy
{
    std::uint8_t available;
    inline bool operator==(const energy& other) const
    {
	return this->available == other.available;
    }
};

template <std::size_t length_c>
turbo::memory::slab_unique_ptr<std::array<energy, length_c>> make_energy(energy initial_value);

struct TURBO_SYMBOL_DECL inventory_config
{
    std::uint8_t min_energy;
    std::uint8_t max_energy;
};

class TURBO_SYMBOL_DECL inventory
{
public:
private:
};

typedef robocup2Dsim::runtime::table<
		robocup2Dsim::runtime::primitives::key_16,
		std::unique_ptr<inventory>,
		robocup2Dsim::runtime::primitives::fixed_cstring_32>
	inventory_table_type;

void register_system(robocup2Dsim::runtime::ecs_db& db, std::unique_ptr<inventory> inv);

} // namespace engine
} // namespace robocup2Dsim

#endif

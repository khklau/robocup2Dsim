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
    std::uint8_t quantity;
    inline bool operator==(const energy& other) const
    {
	return this->quantity == other.quantity;
    }
};

template <std::size_t length_c>
turbo::memory::slab_unique_ptr<std::array<energy, length_c>> make_energy(energy initial_value);

struct TURBO_SYMBOL_DECL inventory_policy
{
    energy minimum;
    energy maximum;
};

class TURBO_SYMBOL_DECL inventory
{
public:
    enum class accrue_result
    {
	success,
	oversupply
    };
    enum class spend_result
    {
	success,
	understock
    };
    inventory(const inventory_policy& policy);
    template <class item_t, std::size_t length_c>
    std::array<accrue_result, length_c> accrue(
	    std::array<item_t, length_c>& stock,
	    const std::array<item_t, length_c>& amount) const;
    template <class item_t, std::size_t length_c>
    std::array<spend_result, length_c> spend(
	    std::array<item_t, length_c>& stock,
	    const std::array<item_t, length_c>& amount) const;
private:
    inventory_policy policy_;
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

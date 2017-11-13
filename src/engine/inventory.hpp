#ifndef ROBOCUP2DSIM_ENGINE_INVENTORY_HPP
#define ROBOCUP2DSIM_ENGINE_INVENTORY_HPP

#include <cstdint>
#include <robocup2Dsim/runtime/ecs_db.hpp>
#include <robocup2Dsim/runtime/ram_db.hpp>

namespace robocup2Dsim {
namespace engine {

struct energy
{
    std::uint8_t available;
};

class inventory
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

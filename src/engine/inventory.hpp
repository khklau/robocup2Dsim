#ifndef ROBOCUP2DSIM_ENGINE_INVENTORY_HPP
#define ROBOCUP2DSIM_ENGINE_INVENTORY_HPP

#include <cstdint>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace engine {
namespace inventory {

struct energy
{
    std::uint8_t available;
};

void register_components(robocup2Dsim::runtime::ecs_db& db);

} // namespace inventory
} // namespace engine
} // namespace robocup2Dsim

#endif

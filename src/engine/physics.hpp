#ifndef ROBOCUP2DSIM_ENGINE_SIMULATION_HPP
#define ROBOCUP2DSIM_ENGINE_SIMULATION_HPP

#include <memory>
#include <Box2D/Dynamics/b2World.h>
#include <turbo/toolset/attribute.hpp>
#include "ram_db.hpp"

namespace robocup2Dsim {
namespace engine {

class TURBO_SYMBOL_DECL simulation
{
public:
    simulation();
private:
    b2World world_;
};

typedef table<primitives::key_16, std::unique_ptr<simulation>, primitives::fixed_cstring_32> physics_table_type;

class ecs_db;

void register_system(ecs_db& db, std::unique_ptr<simulation> sim);

} // namespace engine
} // namespace robobup2Dsim

#endif

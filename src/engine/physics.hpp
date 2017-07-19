#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HPP
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HPP

#include <memory>
#include <Box2D/Dynamics/b2World.h>
#include <turbo/toolset/attribute.hpp>
#include "ram_db.hpp"

namespace robocup2Dsim {
namespace engine {

class TURBO_SYMBOL_DECL physics
{
public:
    physics();
private:
    b2World world_;
};

typedef table<primitives::key_16, std::unique_ptr<physics>, primitives::fixed_cstring_32> physics_table_type;

class ecs_db;

void register_system(ecs_db& db, std::unique_ptr<physics> phys);

} // namespace engine
} // namespace robobup2Dsim

#endif

#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HPP
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HPP

#include <memory>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>
#include <turbo/toolset/attribute.hpp>
#include <robocup2Dsim/runtime/primitives.hpp>
#include <robocup2Dsim/runtime/ram_db.hpp>

namespace robocup2Dsim {

namespace runtime {

class ecs_db;

} // namespace runtime

namespace engine {

class TURBO_SYMBOL_DECL physics
{
public:
    typedef b2Fixture fixture;
    physics();
private:
    b2World world_;
};

typedef robocup2Dsim::runtime::table<
		robocup2Dsim::runtime::primitives::key_16,
		std::unique_ptr<physics>,
		robocup2Dsim::runtime::primitives::fixed_cstring_32>
	physics_table_type;

void register_system(robocup2Dsim::runtime::ecs_db& db, std::unique_ptr<physics> phys);

} // namespace engine
} // namespace robobup2Dsim

#endif

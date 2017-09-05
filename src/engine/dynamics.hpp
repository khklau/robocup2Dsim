#ifndef ROBOCUP2DSIM_ENGINE_DYNAMICS_HPP
#define ROBOCUP2DSIM_ENGINE_DYNAMICS_HPP

#include <Box2D/Dynamics/b2Body.h>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace engine {
namespace dynamics {

typedef b2Body body;
typedef b2BodyDef body_config;

void register_components(robocup2Dsim::runtime::ecs_db& db);

} // namespace dynamics
} // namespace engine
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_COMMON_BALL_HPP
#define ROBOCUP2DSIM_COMMON_BALL_HPP

#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace common {

struct ball_components
{
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> body;
};

ball_components make_ball(robocup2Dsim::runtime::ecs_db& db, const robocup2Dsim::engine::physics::vec2& position);

} // namespace common
} // robocup2Dsim

#endif

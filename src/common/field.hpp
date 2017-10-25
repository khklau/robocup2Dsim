#ifndef ROBOCUP2DSIM_COMMON_FIELD_HPP
#define ROBOCUP2DSIM_COMMON_FIELD_HPP

#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace common {

struct field
{
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> left_goal;
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> right_goal;
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> center_circle;
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> boundary;
};

field make_field(robocup2Dsim::runtime::ecs_db& db);

} // namespace common
} // namespace robocup2Dsim

#endif

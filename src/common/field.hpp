#ifndef ROBOCUP2DSIM_COMMON_FIELD_HPP
#define ROBOCUP2DSIM_COMMON_FIELD_HPP

#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace common {

struct field
{
    robocup2Dsim::engine::dynamics::body left_goal;
    robocup2Dsim::engine::dynamics::body right_goal;
    robocup2Dsim::engine::dynamics::body center_circle;
    robocup2Dsim::engine::dynamics::body boundary;
    robocup2Dsim::engine::dynamics::body left_penalty_box;
    robocup2Dsim::engine::dynamics::body right_penalty_box;

    field() = default;
    field(const field&) = delete;
    field(field&&) = delete;
    ~field();
    field& operator=(const field&) = delete;
    field& operator=(field&&) = delete;
};

void init_field(field& output, robocup2Dsim::runtime::ecs_db& db);

} // namespace common
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_COMMON_STATE_HPP
#define ROBOCUP2DSIM_COMMON_STATE_HPP

#include <robocup2Dsim/common/ball.hpp>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/common/player.hpp>
#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/inventory.hpp>

namespace robocup2Dsim {

namespace runtime {

class ecs_db;

} // namespace runtime

namespace common {

struct sim_state
{
    std::array<engine::energy, entity::MAX_ROSTER_SIZE> stock;
    std::array<player_body, entity::MAX_ROSTER_SIZE> players;
    ball_component ball;

    sim_state(robocup2Dsim::runtime::ecs_db& db);
};

} // namespace common
} // namepsace robocup2Dsim

#endif

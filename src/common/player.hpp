#ifndef ROBOCUP2DSIM_COMMON_PLAYER_HPP
#define ROBOCUP2DSIM_COMMON_PLAYER_HPP

#include <cstdint>
#include <string>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace common {

struct player_body
{
    robocup2Dsim::engine::dynamics::body torso;
    robocup2Dsim::engine::dynamics::body head;
    robocup2Dsim::engine::dynamics::body foot;

    player_body() = default;
    player_body(const player_body&) = delete;
    player_body(player_body&&) = delete;
    ~player_body();
    player_body& operator=(const player_body&) = delete;
    player_body& operator=(player_body&&) = delete;
};

void make_player(
        player_body& output,
	robocup2Dsim::runtime::ecs_db& db,
	const entity::player_id id,
	const robocup2Dsim::engine::physics::vec2& position,
	const std::uint16_t angle_degree);

} // namespace common
} // namespace robocup2Dsim

#endif

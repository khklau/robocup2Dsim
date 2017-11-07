#ifndef ROBOCUP2DSIM_COMMON_PLAYER_HPP
#define ROBOCUP2DSIM_COMMON_PLAYER_HPP

#include <cstdint>
#include <string>
#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace common {

struct player_components
{
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> torso;
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> head;
    robocup2Dsim::engine::physics_ptr<robocup2Dsim::engine::dynamics::body> foot;
};

player_components make_player(
	robocup2Dsim::runtime::ecs_db& db,
	const std::string& name,
	const robocup2Dsim::engine::physics::vec2& position,
	const std::uint16_t angle_degree);

} // namespace common
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_COMMON_ENTITY_HPP
#define ROBOCUP2DSIM_COMMON_ENTITY_HPP

#include <robocup2Dsim/common/entity.capnp.h>
#include <cstdint>

namespace robocup2Dsim {
namespace common {
namespace entity {

enum class collision_category : std::uint8_t
{
    ball,
    goal_post,
    goal_net,
    ball_sensor,
    player_torso,
    player_foot,
    player_head,
    player_sensor,
    marker_sensor,
    max = marker_sensor
};

enum class fixture_name : std::uint16_t
{
    ball,
    top_post,
    bottom_post,
    back_net,
    top_net,
    bottom_net,
    goal_line_sensor,
    center_circle_spot,
    center_circle_top,
    center_circle_left,
    center_circle_right,
    center_circle_bottom,
    boundary_marker_top_left,
    boundary_marker_top_center,
    boundary_marker_top_right,
    boundary_marker_bottom_left,
    boundary_marker_bottom_center,
    boundary_marker_bottom_right,
    boundary_line_top,
    boundary_line_bottom,
    boundary_line_left_top,
    boundary_line_left_bottom,
    boundary_line_right_top,
    boundary_line_right_bottom,
};

enum class team_id : uint8_t
{
    alpha,
    beta
};

struct player_id
{
    UniformNumber uniform;
    team_id team;
    inline bool operator==(const player_id& other) const { return uniform == other.uniform && team == other.team; }
};

} // namespace entity
} // namespace common
} // namespace robocup2Dsim

#endif

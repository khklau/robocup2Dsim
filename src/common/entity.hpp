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
    goal_line,
    center_marker,
    top_marker,
    left_marker,
    right_marker,
    bottom_marker,
    top_left_marker,
    top_center_marker,
    top_right_marker,
    bottom_left_marker,
    bottom_center_marker,
    bottom_right_marker,
    top_line,
    bottom_line,
    left_top_line,
    left_bottom_line,
    right_top_line,
    right_bottom_line
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

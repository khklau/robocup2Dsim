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
    goal_sensor,
    player_torso,
    player_foot,
    player_head,
    player_sensor,
    field_marker,
    field_sensor,
    max = field_sensor
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
    center_circle_bottom
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

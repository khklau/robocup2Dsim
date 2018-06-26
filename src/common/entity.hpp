#ifndef ROBOCUP2DSIM_COMMON_ENTITY_HPP
#define ROBOCUP2DSIM_COMMON_ENTITY_HPP

#include <robocup2Dsim/common/entity.capnp.h>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace robocup2Dsim {
namespace common {
namespace entity {

enum class collision_category : std::uint8_t
{
    ball,
    goal_post,
    goal_net,
    ball_sensor,
    player_body,
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
    right_bottom_line,
    torso,
    foot,
    head,
    vision
};

typedef std::uint8_t player_id;

static const player_id no_player = std::numeric_limits<player_id>::max();

struct old_player_id
{
    UniformNumber uniform;
    TeamId team;
    inline bool operator==(const old_player_id& other) const { return uniform == other.uniform && team == other.team; }
};

inline UniformNumber uint_to_uniform(unsigned int number)
{
    switch (number)
    {
	case 1:
	    return UniformNumber::ONE;
	case 2:
	    return UniformNumber::TWO;
	case 3:
	    return UniformNumber::THREE;
	case 4:
	    return UniformNumber::FOUR;
	case 5:
	    return UniformNumber::FIVE;
	case 6:
	    return UniformNumber::SIX;
	case 7:
	    return UniformNumber::SEVEN;
	case 8:
	    return UniformNumber::EIGHT;
	case 9:
	    return UniformNumber::NINE;
	case 10:
	    return UniformNumber::TEN;
	case 11:
	    return UniformNumber::ELEVEN;
	default:
	    throw std::out_of_range("Not a valid uniform number");
    }
}

} // namespace entity
} // namespace common
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_COMMON_ENTITY_HPP
#define ROBOCUP2DSIM_COMMON_ENTITY_HPP

#include <robocup2Dsim/common/entity.capnp.h>
#include <turbo/type_utility/enum_metadata.hpp>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <tuple>

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

inline UniformNumber uint_to_uniform(std::uint8_t number)
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

inline player_id uniform_to_id(UniformNumber uniform, TeamId team)
{
    // Player IDs start at index 0
    player_id index = 0U;
    switch (uniform)
    {
        case UniformNumber::ONE:
            index = 0U;
            break;
        case UniformNumber::TWO:
            index = 1U;
            break;
        case UniformNumber::THREE:
            index = 2U;
            break;
        case UniformNumber::FOUR:
            index = 3U;
            break;
        case UniformNumber::FIVE:
            index = 4U;
            break;
        case UniformNumber::SIX:
            index = 5U;
            break;
        case UniformNumber::SEVEN:
            index = 6U;
            break;
        case UniformNumber::EIGHT:
            index = 7U;
            break;
        case UniformNumber::NINE:
            index = 8U;
            break;
        case UniformNumber::TEN:
            index = 9U;
            break;
        case UniformNumber::ELEVEN:
        default:
            index = 10U;
            break;
    }
    constexpr std::size_t max_team_size = turbo::type_utility::enum_count(
            robocup2Dsim::common::entity::UniformNumber::ONE,
            robocup2Dsim::common::entity::UniformNumber::ELEVEN);
    switch (team)
    {
        case TeamId::ALPHA:
            return 0U + index;
        case TeamId::BETA:
        default:
            return max_team_size + index;
    }
}

inline std::tuple<UniformNumber, TeamId> id_to_uniform(player_id player)
{
    constexpr std::size_t max_team_size = turbo::type_utility::enum_count(
            robocup2Dsim::common::entity::UniformNumber::ONE,
            robocup2Dsim::common::entity::UniformNumber::ELEVEN);
    TeamId team = TeamId::ALPHA;
    player_id index = player;
    if (max_team_size <= index)
    {
        team = TeamId::BETA;
        index -= max_team_size;
    }
    else
    {
        // for clarity about how ALPHA players are handled
        team = TeamId::ALPHA;
        index -= 0U;
    }
    switch (index)
    {
        // Player IDs start at index 0
        case 0U:
            return std::make_tuple(UniformNumber::ONE, team);
        case 1U:
            return std::make_tuple(UniformNumber::TWO, team);
        case 2U:
            return std::make_tuple(UniformNumber::THREE, team);
        case 3U:
            return std::make_tuple(UniformNumber::FOUR, team);
        case 4U:
            return std::make_tuple(UniformNumber::FIVE, team);
        case 5U:
            return std::make_tuple(UniformNumber::SIX, team);
        case 6U:
            return std::make_tuple(UniformNumber::SEVEN, team);
        case 7U:
            return std::make_tuple(UniformNumber::EIGHT, team);
        case 8U:
            return std::make_tuple(UniformNumber::NINE, team);
        case 9U:
            return std::make_tuple(UniformNumber::TEN, team);
        case 10U:
        default:
            return std::make_tuple(UniformNumber::ELEVEN, team);
    }
}

} // namespace entity
} // namespace common
} // namespace robocup2Dsim

#endif

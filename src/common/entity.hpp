#ifndef ROBOCUP2DSIM_COMMON_ENTITY_HPP
#define ROBOCUP2DSIM_COMMON_ENTITY_HPP

#include <robocup2Dsim/common/entity.capnp.h>
#include <cstdint>

namespace robocup2Dsim {
namespace common {
namespace entity {

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

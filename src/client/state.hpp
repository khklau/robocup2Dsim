#ifndef ROBOCUP2DSIM_CLIENT_STATE_HPP
#define ROBOCUP2DSIM_CLIENT_STATE_HPP

#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/field.hpp>
#include <robocup2Dsim/common/state.hpp>

namespace robocup2Dsim {
namespace client {

struct player_config
{
    common::entity::player_id player_id;
    common::entity::UniformNumber uniform;
    common::entity::TeamId team;
    common::entity::PlayerType player_type;
};

struct client_game_state
{
    common::sim_state sim;
    common::field field;
    player_config player;

    inline client_game_state(
            robocup2Dsim::runtime::ecs_db& db,
            common::entity::UniformNumber uniform,
            common::entity::TeamId team,
            common::entity::PlayerType player_type)
        :
            sim(db),
            field(db),
            player{common::entity::uniform_to_id(uniform, team), uniform, team, player_type}
    { } 
};

}
}

#endif

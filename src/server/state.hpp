#ifndef ROBOCUP2DSIM_SERVER_STATE_HPP
#define ROBOCUP2DSIM_SERVER_STATE_HPP

#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/field.hpp>
#include <robocup2Dsim/common/state.hpp>

namespace robocup2Dsim {
namespace server {

struct server_game_state
{
    common::sim_state sim;
    common::field field;

    inline server_game_state(robocup2Dsim::runtime::ecs_db& db)
        :
            sim(db),
            field(db)
    { } 
};

}
}

#endif


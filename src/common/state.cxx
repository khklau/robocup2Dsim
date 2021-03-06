#include "state.hpp"
#include <robocup2Dsim/runtime/ecs_db.hh>
#include <turbo/type_utility/enum_iterator.hh>

namespace ttu = turbo::type_utility;

namespace robocup2Dsim {
namespace common {

sim_state::sim_state(robocup2Dsim::runtime::ecs_db& db)
    :
        stock(),
        players(),
        ball(db, engine::physics::vec2(0, 340))
{
    float direction = -1;
    for (auto team : ttu::enum_iterator<entity::TeamId, entity::TeamId::ALPHA, entity::TeamId::BETA>())
    {
        const float y_position = 2;
        float x_position = 340;
        for (auto uniform : ttu::enum_iterator<entity::UniformNumber, entity::UniformNumber::ONE, entity::UniformNumber::ELEVEN>())
        {
            entity::player_id index = entity::uniform_to_id(uniform, team);
            std::string description = entity::uniform_to_string(uniform, team);

            stock[index] = engine::energy{0U};
            init_player(
                    players[index],
                    db,
                    index,
                    engine::physics::vec2(x_position * direction, y_position),
                    90U);
            x_position -= 20;
        }
        direction *= -1;
    }
}

} // namespace common
} // namespace robocup2Dsim

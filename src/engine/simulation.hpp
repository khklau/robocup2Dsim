#ifndef ROBOCUP2DSIM_ENGINE_SIMULATION_HPP
#define ROBOCUP2DSIM_ENGINE_SIMULATION_HPP

#include <Box2D/Dynamics/b2World.h>

namespace robocup2Dsim {
namespace engine {

class simulation
{
public:
    simulation();
private:
    b2World world_;
};

} // namespace engine
} // namespace robobup2Dsim

#endif

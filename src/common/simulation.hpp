#ifndef ROBOCUP2DSIM_COMMON_SIMULATION_HPP
#define ROBOCUP2DSIM_COMMON_SIMULATION_HPP

#include <Box2D/Dynamics/b2World.h>

namespace robocup2Dsim {
namespace common {

class simulation
{
public:
    simulation();
private:
    b2World world_;
};

} // namespace common
} // namespace robobup2Dsim

#endif

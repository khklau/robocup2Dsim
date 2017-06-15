#include "simulation.hpp"
#include <Box2D/Common/b2Math.h>
#include <SnapBox2D/snapshot.hpp>
#include <SnapBox2D/storage.hpp>

namespace robocup2Dsim {
namespace common {

simulation::simulation()
    :
	world_(b2Vec2(0, 0))
{ }

} // namespace common
} // robocup2Dsim

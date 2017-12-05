#include "action.hpp"
#include <cmath>

namespace ren = robocup2Dsim::engine;

namespace robocup2Dsim {
namespace common {

void act(
	ren::physics& physics,
	const ren::dynamics::body& torso,
	ren::dynamics::body& foot,
	const MoveFootAction::Reader& action)
{
    ren::physics::vec2 target(0, 0);
    target.x = action.getVelocity() / 1000.0 * std::cos(torso.GetAngle());
    target.y = action.getVelocity() / 1000.0 * std::sin(torso.GetAngle());
    physics.apply_linear_impulse(foot, target);
}

void act(
	robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& torso,
	robocup2Dsim::engine::dynamics::body& head,
	robocup2Dsim::engine::dynamics::body& foot,
	const RunAction::Reader& action)
{
    ren::physics::vec2 target(0, 0);
    target.x = action.getVelocity() / 1000.0 * std::cos(torso.GetAngle());
    target.y = action.getVelocity() / 1000.0 * std::sin(torso.GetAngle());
    physics.apply_linear_impulse(torso, target);
    physics.apply_linear_impulse(head, target);
    physics.apply_linear_impulse(foot, target);
}

void act(
	robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& head,
	const TurnHeadAction::Reader& action)
{
    physics.apply_angular_impulse(head, action.getVelocity() / 1000.0);
}

void act(
	robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& torso,
	robocup2Dsim::engine::dynamics::body& foot,
	const TurnTorsoAction::Reader& action)
{
    physics.apply_angular_impulse(torso, action.getVelocity() / 1000.0);
    physics.apply_angular_impulse(foot, action.getVelocity() / 1000.0);
}

} // namespace common
} // namespace robocup2Dsim

#include "action.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <robocup2Dsim/engine/physics.hxx>
#include <robocup2Dsim/engine/inventory.hxx>

namespace ren = robocup2Dsim::engine;

namespace robocup2Dsim {
namespace common {

ren::inventory::spend_result act(
	const ren::inventory& inventory,
	ren::energy& stock,
	const ren::physics& physics,
	const ren::dynamics::body& torso,
	ren::dynamics::body& foot,
	const MoveFootAction::Reader& action)
{
    // Need to work around a strange unary minus return type bug in Gcc
    decltype(MoveFootAction::MAX_VELOCITY) velocity = (action.getVelocity() > 0)
	    ? std::min(action.getVelocity(), MoveFootAction::MAX_VELOCITY)
	    : std::max(action.getVelocity(), static_cast<decltype(MoveFootAction::MAX_VELOCITY)>(-MoveFootAction::MAX_VELOCITY));
    // Need to be careful to avoid overflow
    robocup2Dsim::engine::energy cost{static_cast<decltype(cost.quantity)>(std::lround(
	    (std::abs(velocity) / static_cast<double>(MoveFootAction::MAX_VELOCITY))
	    * static_cast<double>(MoveFootAction::MAX_COST)))};
    ren::inventory::spend_result result = inventory.spend(stock, cost);
    if (result == ren::inventory::spend_result::success)
    {
	ren::physics::vec2 target(0, 0);
	target.x = velocity / VELOCITY_SCALE_FACTOR * std::cos(torso.GetAngle());
	target.y = velocity / VELOCITY_SCALE_FACTOR * std::sin(torso.GetAngle());
	physics.apply_linear_impulse(foot, target);
    }
    return result;
}

ren::inventory::spend_result act(
	const ren::inventory& inventory,
	ren::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& torso,
	robocup2Dsim::engine::dynamics::body& head,
	robocup2Dsim::engine::dynamics::body& foot,
	const RunAction::Reader& action)
{
    decltype(RunAction::MAX_FORWARD_VELOCITY) velocity = (action.getVelocity() > 0)
	    ? std::min(action.getVelocity(), RunAction::MAX_FORWARD_VELOCITY)
	    : std::max(action.getVelocity(), RunAction::MAX_BACKWARD_VELOCITY);
    // Need to be careful to avoid overflow
    robocup2Dsim::engine::energy cost{static_cast<decltype(cost.quantity)>((action.getVelocity() > 0)
	    ? std::lround((velocity / static_cast<double>(RunAction::MAX_FORWARD_VELOCITY)) * static_cast<double>(RunAction::MAX_FORWARD_COST))
	    : std::lround(std::abs(velocity / static_cast<double>(RunAction::MAX_BACKWARD_VELOCITY)) * static_cast<double>(RunAction::MAX_BACKWARD_COST)))};
    ren::inventory::spend_result result = inventory.spend(stock, cost);
    if (result == ren::inventory::spend_result::success)
    {
	ren::physics::vec2 target(0, 0);
	target.x = velocity / VELOCITY_SCALE_FACTOR * std::cos(torso.GetAngle());
	target.y = velocity / VELOCITY_SCALE_FACTOR * std::sin(torso.GetAngle());
	physics.apply_linear_impulse(torso, target);
	physics.apply_linear_impulse(head, target);
	physics.apply_linear_impulse(foot, target);
    }
    return result;
}

void act(
	const robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& head,
	const TurnHeadAction::Reader& action)
{
    physics.apply_angular_impulse(head, action.getVelocity() / VELOCITY_SCALE_FACTOR);
}

void act(
	const robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& torso,
	robocup2Dsim::engine::dynamics::body& foot,
	const TurnTorsoAction::Reader& action)
{
    physics.apply_angular_impulse(torso, action.getVelocity() / VELOCITY_SCALE_FACTOR);
    physics.apply_angular_impulse(foot, action.getVelocity() / VELOCITY_SCALE_FACTOR);
}

} // namespace common
} // namespace robocup2Dsim

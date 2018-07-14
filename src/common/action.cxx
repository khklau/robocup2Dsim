#include "action.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <robocup2Dsim/engine/physics.hh>
#include <robocup2Dsim/engine/inventory.hh>

namespace ren = robocup2Dsim::engine;

namespace robocup2Dsim {
namespace common {

ren::inventory::spend_result act(
	const ren::inventory& inventory,
	ren::energy& stock,
	const ren::physics& physics,
	player_body& body,
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
	target.x = velocity / VELOCITY_SCALE_FACTOR * std::cos(body.torso->GetAngle());
	target.y = velocity / VELOCITY_SCALE_FACTOR * std::sin(body.torso->GetAngle());
	physics.apply_linear_impulse(*(body.foot), target);
    }
    return result;
}

ren::inventory::spend_result act(
	const ren::inventory& inventory,
	ren::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	player_body& body,
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
	target.x = velocity / VELOCITY_SCALE_FACTOR * std::cos(body.torso->GetAngle());
	target.y = velocity / VELOCITY_SCALE_FACTOR * std::sin(body.torso->GetAngle());
	physics.apply_linear_impulse(*(body.torso), target);
	physics.apply_linear_impulse(*(body.head), target);
	physics.apply_linear_impulse(*(body.foot), target);
    }
    return result;
}

ren::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	player_body& body,
	const TurnHeadAction::Reader& action)
{
    // Need to work around a strange unary minus return type bug in Gcc
    decltype(TurnHeadAction::MAX_VELOCITY) velocity = (action.getVelocity() > 0)
	    ? std::min(action.getVelocity(), TurnHeadAction::MAX_VELOCITY)
	    : std::max(action.getVelocity(), static_cast<decltype(TurnHeadAction::MAX_VELOCITY)>(-TurnHeadAction::MAX_VELOCITY));
    // Need to be careful to avoid overflow
    robocup2Dsim::engine::energy cost{static_cast<decltype(cost.quantity)>(std::lround(
	    (std::abs(velocity) / static_cast<double>(TurnHeadAction::MAX_VELOCITY))
	    * static_cast<double>(TurnHeadAction::MAX_COST)))};
    ren::inventory::spend_result result = inventory.spend(stock, cost);
    if (result == ren::inventory::spend_result::success)
    {
	physics.apply_angular_impulse(*(body.head), velocity / VELOCITY_SCALE_FACTOR);
    }
    return result;
}

ren::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	player_body& body,
	const TurnTorsoAction::Reader& action)
{
    // Need to work around a strange unary minus return type bug in Gcc
    decltype(TurnTorsoAction::MAX_VELOCITY) velocity = (action.getVelocity() > 0)
	    ? std::min(action.getVelocity(), TurnTorsoAction::MAX_VELOCITY)
	    : std::max(action.getVelocity(), static_cast<decltype(TurnTorsoAction::MAX_VELOCITY)>(-TurnTorsoAction::MAX_VELOCITY));
    // Need to be careful to avoid overflow
    robocup2Dsim::engine::energy cost{static_cast<decltype(cost.quantity)>(std::lround(
	    (std::abs(velocity) / static_cast<double>(TurnTorsoAction::MAX_VELOCITY))
	    * static_cast<double>(TurnTorsoAction::MAX_COST)))};
    ren::inventory::spend_result result = inventory.spend(stock, cost);
    if (result == ren::inventory::spend_result::success)
    {
	physics.apply_angular_impulse(*(body.torso), action.getVelocity() / VELOCITY_SCALE_FACTOR);
	physics.apply_angular_impulse(*(body.foot), action.getVelocity() / VELOCITY_SCALE_FACTOR);
    }
    return result;
}

} // namespace common
} // namespace robocup2Dsim

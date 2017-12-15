#ifndef ROBOCUP2DSIM_COMMON_ACTION_HPP
#define ROBOCUP2DSIM_COMMON_ACTION_HPP

#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/inventory.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/common/action.capnp.h>

namespace robocup2Dsim {
namespace common {

robocup2Dsim::engine::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	const robocup2Dsim::engine::dynamics::body& torso,
	robocup2Dsim::engine::dynamics::body& foot,
	const MoveFootAction::Reader& action);

robocup2Dsim::engine::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& torso,
	robocup2Dsim::engine::dynamics::body& head,
	robocup2Dsim::engine::dynamics::body& foot,
	const RunAction::Reader& action);

void act(
	const robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& head,
	const TurnHeadAction::Reader& action);

void act(
	const robocup2Dsim::engine::physics& physics,
	robocup2Dsim::engine::dynamics::body& torso,
	robocup2Dsim::engine::dynamics::body& foot,
	const TurnTorsoAction::Reader& action);

} // namespace common
} // namespace robocup2Dsim

#endif

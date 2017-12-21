#ifndef ROBOCUP2DSIM_COMMON_ACTION_HPP
#define ROBOCUP2DSIM_COMMON_ACTION_HPP

#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/inventory.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/common/action.capnp.h>
#include <robocup2Dsim/common/player.hpp>

namespace robocup2Dsim {
namespace common {

robocup2Dsim::engine::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	player_body& body,
	const MoveFootAction::Reader& action);

robocup2Dsim::engine::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	player_body& body,
	const RunAction::Reader& action);

robocup2Dsim::engine::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	player_body& body,
	const TurnHeadAction::Reader& action);

robocup2Dsim::engine::inventory::spend_result act(
	const robocup2Dsim::engine::inventory& inventory,
	robocup2Dsim::engine::energy& stock,
	const robocup2Dsim::engine::physics& physics,
	player_body& body,
	const TurnTorsoAction::Reader& action);

} // namespace common
} // namespace robocup2Dsim

#endif

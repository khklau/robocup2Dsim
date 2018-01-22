#ifndef ROBOCUP2DSIM_COMMON_ACTION_HPP
#define ROBOCUP2DSIM_COMMON_ACTION_HPP

#include <memory>
#include <vector>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/inventory.hpp>
#include <robocup2Dsim/engine/physics.hpp>
#include <robocup2Dsim/common/action.capnp.h>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/common/player.hpp>

namespace robocup2Dsim {
namespace common {

struct client_action
{
    PlayerAction::Reader action;
    entity::player_id player;
    inline client_action(PlayerAction::Reader&& arg1, entity::player_id arg2)
	:
	    action(arg1), player(arg2)
    { }
};

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

template <std::size_t roster_size_c>
void act(
	const robocup2Dsim::engine::inventory& inventory,
	std::array<robocup2Dsim::engine::energy, roster_size_c>& stock,
	const robocup2Dsim::engine::physics& physics,
	std::array<player_body, roster_size_c>& body,
	const std::vector<client_action>& action_list,
	std::vector<robocup2Dsim::engine::inventory::spend_result>& result_list);

} // namespace common
} // namespace robocup2Dsim

#endif

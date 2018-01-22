#ifndef ROBOCUP2DSIM_COMMON_ACTION_HXX
#define ROBOCUP2DSIM_COMMON_ACTION_HXX

#include <turbo/toolset/extension.hpp>

namespace robocup2Dsim {
namespace common {

template <std::size_t roster_size_c>
void act(
	const robocup2Dsim::engine::inventory& inventory,
	std::array<robocup2Dsim::engine::energy, roster_size_c>& stock,
	const robocup2Dsim::engine::physics& physics,
	std::array<player_body, roster_size_c>& body,
	const std::vector<client_action>& action_list,
	std::vector<robocup2Dsim::engine::inventory::spend_result>& result_list)
{
    result_list.clear();
    for (const client_action& message: action_list)
    {
	if (TURBO_UNLIKELY(message.player >= roster_size_c))
	{
	    // TODO: log a warning?
	    result_list.push_back(robocup2Dsim::engine::inventory::spend_result::understock);
	    continue;
	}
	switch (message.action.getAction().which())
	{
	    case PlayerAction::Action::MOVE_FOOT:
		result_list.push_back(act(
			inventory,
			stock[message.player],
			physics,
			body[message.player],
			message.action.getAction().getMoveFoot()));
		break;
	    case PlayerAction::Action::CATCH:
		// TODO: implement later
		result_list.push_back(robocup2Dsim::engine::inventory::spend_result::success);
		break;
	    case PlayerAction::Action::RUN:
		result_list.push_back(act(
			inventory,
			stock[message.player],
			physics,
			body[message.player],
			message.action.getAction().getRun()));
		break;
	    case PlayerAction::Action::TURN_HEAD:
		result_list.push_back(act(
			inventory,
			stock[message.player],
			physics,
			body[message.player],
			message.action.getAction().getTurnHead()));
		break;
	    case PlayerAction::Action::TURN_TORSO:
		result_list.push_back(act(
			inventory,
			stock[message.player],
			physics,
			body[message.player],
			message.action.getAction().getTurnTorso()));
		break;
	}
    }
}

} // namespace common
} // namespace robocup2Dsim

#endif

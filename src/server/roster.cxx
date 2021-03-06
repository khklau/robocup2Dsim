#include "roster.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <turbo/type_utility/enum_iterator.hpp>
#include <turbo/type_utility/enum_iterator.hh>
#include <robocup2Dsim/common/metadata.capnp.h>
#include <robocup2Dsim/common/metadata.hpp>

namespace bin = beam::internet;
namespace rcc = robocup2Dsim::common::command;
namespace rce = robocup2Dsim::common::entity;
namespace rcm = robocup2Dsim::common::metadata;
namespace ttu = turbo::type_utility;

namespace robocup2Dsim {
namespace server {

roster::const_iterator::const_iterator(
        const team_list_type& teams,
        const player_list_type& players,
        player_list_type::const_iterator iterator)
    :
        teams_(&teams),
        players_(&players),
        iterator_(iterator)
{ }

rce::player_id roster::const_iterator::get_player_id() const
{
    if (players_->cend() == iterator_)
    {
        throw std::out_of_range("iterator exceeds the player range in the roster");
    }
    return iterator_ - players_->cbegin();
}

roster::roster(
	const roster::player_list_type& enrolled_players,
	const roster::team_list_type& enrolled_teams,
	const roster::goalie_list_type& enrolled_goalies)
{
    std::copy_n(enrolled_players.cbegin(), enrolled_players.max_size(), players_.begin());
    // not safe to use memcpy for arrays of strings
    std::copy(enrolled_teams.cbegin(), enrolled_teams.cend(), teams_.begin());
    std::copy_n(enrolled_goalies.cbegin(), enrolled_goalies.max_size(), goalies_.begin());
}

deregister_result roster::deregister_client(const bin::endpoint_id& client)
{
    auto result = find_player(client);
    if (std::get<0>(result) == find_result::found)
    {
	auto iter = std::find(goalies_.begin(), goalies_.end(), std::get<1>(result));
	if (iter != goalies_.end())
	{
	    // deregister the goalie
	    *iter = rce::no_player;
	}
	players_[std::get<1>(result)] = bin::endpoint_id();
	return deregister_result::success;
    }
    else
    {
	return deregister_result::client_not_found;
    }
}

std::string roster::get_team_name(const rce::TeamId& team) const
{
    switch (team)
    {
	case rce::TeamId::ALPHA:
	    return teams_.cbegin()->name;
	default:
	    return teams_.crbegin()->name;
    }
}

enrollment::register_result enrollment::register_client(
	const bin::endpoint_id& client,
	const rcc::Registration::Reader& request)
{
    const rcm::Version::Reader version = request.getVersion();
    const rcm::Version::Reader current = *(rcm::CURRENT_VERSION);
    if (version != current)
    {
	return enrollment::register_result::version_mismatch;
    }
    auto team = enrollment_.find(request.getTeamName());
    if (team == enrollment_.end())
    {
	if (enrollment_.size() == 2)
	{
	    return enrollment::register_result::team_slot_taken;
	}
	else
	{
	    team = enrollment_.emplace(std::piecewise_construct, std::make_tuple(request.getTeamName()), std::make_tuple()).first;
	}
    }
    if (team->second.find(request.getUniform()) != team->second.cend())
    {
	return enrollment::register_result::uniform_taken;
    }
    else if (request.getPlayerType() == rce::PlayerType::GOAL_KEEPER
	    && std::count_if(team->second.cbegin(), team->second.cend(), [] (const team::value_type& member) -> bool
	    {
		return member.second.ptype == rce::PlayerType::GOAL_KEEPER;
	    }) > 0)
    {
	return enrollment::register_result::goalie_taken;
    }
    else
    {
	team->second.emplace(
		std::piecewise_construct,
		std::make_tuple(request.getUniform()),
		std::make_tuple(client, request.getPlayerType()));
	client_player_map_.emplace(
		std::piecewise_construct,
		std::make_tuple(client),
		std::make_tuple(request.getTeamName(), request.getUniform()));
	return enrollment::register_result::success;
    }
}

deregister_result enrollment::deregister_client(const bin::endpoint_id& client)
{
    auto iter = client_player_map_.find(client);
    if (iter != client_player_map_.cend())
    {
	auto team = enrollment_.find(iter->second.team_name);
	if (team != enrollment_.end())
	{
	    team->second.erase(iter->second.uniform);
	}
	client_player_map_.erase(iter);
	return deregister_result::success;
    }
    else
    {
	return deregister_result::client_not_found;
    }
}

std::unique_ptr<roster> enrollment::finalise() const
{
    if (!is_full())
    {
	return std::move(std::unique_ptr<roster>());
    }
    std::array<bin::endpoint_id, rce::MAX_ROSTER_SIZE> player_list;
    std::array<rce::player_id, rce::MAX_CLUB_COUNT> goalie_list;
    rce::player_id goalie_index = 0U;
    for (rce::player_id& goalie: goalie_list)
    {
	goalie = rce::no_player;
    }
    auto finalise_player = [&](rce::player_id player, const client& cl)
    {
        player_list[player] = cl.id;
        if (goalie_index < goalie_list.max_size() && cl.ptype == rce::PlayerType::GOAL_KEEPER)
        {
            goalie_list[goalie_index] = player;
            ++goalie_index;
        }
    };
    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
    {
        rce::player_id alpha_player = rce::uniform_to_id(uniform, rce::TeamId::ALPHA);
        auto& alpha_client = enrollment_.cbegin()->second.at(uniform);
        finalise_player(alpha_player, alpha_client);

        rce::player_id beta_player = rce::uniform_to_id(uniform, rce::TeamId::BETA);
        auto& beta_client = enrollment_.crbegin()->second.at(uniform);
        finalise_player(beta_player, beta_client);
    }
    std::array<roster::team_sheet, rce::MAX_CLUB_COUNT> team_list{{
            { enrollment_.cbegin()->first, 0U, rce::MAX_TEAM_SIZE - 1U },
            { enrollment_.crbegin()->first, rce::MAX_TEAM_SIZE, rce::MAX_ROSTER_SIZE - 1U } }};
    std::unique_ptr<roster> result(new roster(player_list, team_list, goalie_list));
    return std::move(result);
}

bool enrollment::is_registered(const bin::endpoint_id& client) const
{
    return client_player_map_.find(client) != client_player_map_.cend();
}

bool enrollment::is_registered(const std::string& team, robocup2Dsim::common::entity::UniformNumber uniform) const
{
    auto iter = enrollment_.find(team);
    return iter != enrollment_.cend() && iter->second.find(uniform) != iter->second.cend();
}

bool enrollment::is_full() const
{
    bool is_team_full = (enrollment_.size() == rce::MAX_CLUB_COUNT);
    for (auto pair: enrollment_)
    {
	is_team_full &= (pair.second.size() == rce::MAX_TEAM_SIZE);
    }
    return is_team_full;
}

} // namespace server
} // namespace robocup2Dsim

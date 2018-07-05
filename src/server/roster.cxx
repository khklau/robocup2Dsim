#include "roster.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <turbo/type_utility/enum_iterator.hpp>
#include <turbo/type_utility/enum_iterator.hxx>

namespace bin = beam::internet;
namespace rcc = robocup2Dsim::common::command;
namespace rce = robocup2Dsim::common::entity;
namespace rcs = robocup2Dsim::csprotocol;
namespace ttu = turbo::type_utility;

namespace robocup2Dsim {
namespace server {

roster::roster(
	const std::array<bin::endpoint_id, MAX_ROSTER_SIZE>& enrolled_players,
	const std::array<std::string, MAX_CLUB_COUNT>& enrolled_teams,
	const std::array<rce::player_id, MAX_CLUB_COUNT> enrolled_goalies)
{
    std::copy_n(enrolled_players.cbegin(), enrolled_players.max_size(), players_.begin());
    // not safe to use memcpy for arrays of strings
    std::copy(enrolled_teams.cbegin(), enrolled_teams.cend(), team_names_.begin());
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
	    return *(team_names_.cbegin());
	default:
	    return *(team_names_.crbegin());
    }
}

enrollment::register_result enrollment::register_client(
	const bin::endpoint_id& client,
	const rcs::RegistrationRequest::Reader& request)
{
    const rcc::Registration::Reader& detail = request.getDetails();
    auto team = enrollment_.find(detail.getTeamName());
    if (team == enrollment_.end())
    {
	if (enrollment_.size() == 2)
	{
	    return enrollment::register_result::team_slot_taken;
	}
	else
	{
	    team = enrollment_.emplace(std::piecewise_construct, std::make_tuple(detail.getTeamName()), std::make_tuple()).first;
	}
    }
    if (team->second.find(detail.getUniform()) != team->second.cend())
    {
	return enrollment::register_result::uniform_taken;
    }
    else if (detail.getPlayerType() == rce::PlayerType::GOAL_KEEPER
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
		std::make_tuple(detail.getUniform()),
		std::make_tuple(client, detail.getPlayerType()));
	client_player_map_.emplace(
		std::piecewise_construct,
		std::make_tuple(client),
		std::make_tuple(detail.getTeamName(), detail.getUniform()));
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
    std::array<bin::endpoint_id, MAX_ROSTER_SIZE> player_list;
    std::array<rce::player_id, MAX_CLUB_COUNT> goalie_list;
    rce::player_id player_index = 0U;
    rce::player_id goalie_index = 0U;
    for (rce::player_id& goalie: goalie_list)
    {
	goalie = rce::no_player;
    }
    for (const decltype(enrollment_)::value_type& enrolled: enrollment_)
    {
	for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
	{
	    if (player_index < player_list.max_size())
	    {
		player_list[player_index] = enrolled.second.at(uniform).id;
		if (goalie_index < goalie_list.max_size() && enrolled.second.at(uniform).ptype == rce::PlayerType::GOAL_KEEPER)
		{
		    goalie_list[goalie_index] = player_index;
		    ++goalie_index;
		}
		++player_index;
	    }
	}
    }
    std::array<std::string, MAX_CLUB_COUNT> team_list{
	    enrollment_.cbegin()->first,
	    enrollment_.crbegin()->first};
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
    bool is_team_full = (enrollment_.size() == MAX_CLUB_COUNT);
    for (auto pair: enrollment_)
    {
	is_team_full &= (pair.second.size() == MAX_TEAM_SIZE);
    }
    return is_team_full;
}

} // namespace server
} // namespace robocup2Dsim

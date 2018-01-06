#include "roster.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <turbo/type_utility/enum_metadata.hpp>
#include <turbo/type_utility/enum_iterator.hpp>
#include <turbo/type_utility/enum_iterator.hxx>

namespace rcc = robocup2Dsim::common::command;
namespace rce = robocup2Dsim::common::entity;
namespace rcs = robocup2Dsim::csprotocol;
namespace ttu = turbo::type_utility;

namespace robocup2Dsim {
namespace server {

roster::team::team()
{
    team_.fill(rcs::no_client);
}

rcs::client_id& roster::team::operator[](const rce::UniformNumber& uniform)
{
    static rcs::client_id no_client = rcs::no_client;
    switch(uniform)
    {
	case rce::UniformNumber::ONE:
	    return team_[0];
	case rce::UniformNumber::TWO:
	    return team_[1];
	case rce::UniformNumber::THREE:
	    return team_[2];
	case rce::UniformNumber::FOUR:
	    return team_[3];
	case rce::UniformNumber::FIVE:
	    return team_[4];
	case rce::UniformNumber::SIX:
	    return team_[5];
	case rce::UniformNumber::SEVEN:
	    return team_[6];
	case rce::UniformNumber::EIGHT:
	    return team_[7];
	case rce::UniformNumber::NINE:
	    return team_[8];
	case rce::UniformNumber::TEN:
	    return team_[9];
	case rce::UniformNumber::ELEVEN:
	    return team_[10];
	default:
	    return no_client;
    }
}

const rcs::client_id& roster::team::operator[](const rce::UniformNumber& uniform) const
{
    switch(uniform)
    {
	case rce::UniformNumber::ONE:
	    return team_[0];
	case rce::UniformNumber::TWO:
	    return team_[1];
	case rce::UniformNumber::THREE:
	    return team_[2];
	case rce::UniformNumber::FOUR:
	    return team_[3];
	case rce::UniformNumber::FIVE:
	    return team_[4];
	case rce::UniformNumber::SIX:
	    return team_[5];
	case rce::UniformNumber::SEVEN:
	    return team_[6];
	case rce::UniformNumber::EIGHT:
	    return team_[7];
	case rce::UniformNumber::NINE:
	    return team_[8];
	case rce::UniformNumber::TEN:
	    return team_[9];
	default:
	    return team_[10];
    }
}

bool roster::team::is_complete() const
{
    return std::find(team_.cbegin(), team_.cend(), rcs::no_client) == team_.cend();
}

roster::registration_result roster::register_client(const rcs::client_id& client, const rcs::RegistrationRequest::Reader& request)
{
    const rcc::Registration::Reader& detail = request.getDetails();
    auto team = roster_.find(detail.getTeamName());
    if (team == roster_.end())
    {
	if (roster_.size() == 2)
	{
	    return roster::registration_result::team_slot_taken;
	}
	else
	{
	    team = roster_.emplace(std::piecewise_construct, std::make_tuple(detail.getTeamName()), std::make_tuple()).first;
	}
    }
    if (team->second[detail.getUniform()] != rcs::no_client)
    {
	return roster::registration_result::uniform_taken;
    }
    else
    {
	team->second[detail.getUniform()] = client;
	if (is_finalised())
	{
	    if (detail.getTeamName() == get_team_name(rce::TeamId::ALPHA))
	    {
		map_.emplace(client, rce::old_player_id{ detail.getUniform(), rce::TeamId::ALPHA });
	    }
	    else
	    {
		map_.emplace(client, rce::old_player_id{ detail.getUniform(), rce::TeamId::BETA });
	    }
	}
	return roster::registration_result::success;
    }
}

void roster::deregister_client(const robocup2Dsim::csprotocol::client_id& client)
{
    bool finished = false;
    for (auto team = roster_.begin(); !finished && team != roster_.end(); ++team)
    {
	for (auto iter = team->second.begin(); !finished && iter != team->second.end(); ++iter)
	{
	    if (client == *iter)
	    {
		*iter = rcs::no_client;
		if (is_finalised())
		{
		    map_.erase(client);
		}
		finished = true;
	    }
	}
    }
}

bool roster::is_complete() const
{
    return (roster_.size() == 2U) && roster_.cbegin()->second.is_complete() && roster_.crbegin()->second.is_complete();
}

bool roster::is_finalised() const
{
    return map_.size() != 0U;
}

roster::finalisation_result roster::finalise()
{
    client_player_map tmp;
    if (roster_.size() != 2U)
    {
	return finalisation_result::roster_incomplete;
    }
    else
    {
	for (auto id: ttu::enum_iterator<rce::TeamId, rce::TeamId::ALPHA, rce::TeamId::BETA>())
	{
	    team& team_roster = roster_.find(get_team_name(id))->second;
	    for (auto uniform: ttu::enum_iterator<rce::UniformNumber, rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN>())
	    {
		if (team_roster[uniform] == rcs::no_client)
		{
		    return finalisation_result::roster_incomplete;
		}
		else
		{
		    tmp.emplace(team_roster[uniform], rce::old_player_id{uniform, id});
		}
	    }
	}
	map_ = std::move(tmp);
	return finalisation_result::success;
    }
}

bool roster::is_registered(const robocup2Dsim::common::entity::old_player_id& player) const
{
    auto iter = roster_.find(get_team_name(player.team));
    if (iter != roster_.end())
    {
	if (iter->second[player.uniform] != rcs::no_client)
	{
	    return true;
	}
    }
    return false;
}

bool roster::is_registered(const robocup2Dsim::csprotocol::client_id& client) const
{
    return map_.find(client) != map_.end();
}

robocup2Dsim::csprotocol::client_id roster::get_client(const robocup2Dsim::common::entity::old_player_id& player) const
{
    auto iter = roster_.find(get_team_name(player.team));
    if (iter != roster_.end())
    {
	return iter->second[player.uniform];
    }
    else
    {
	throw std::out_of_range("Player is not in roster");
    }
}

robocup2Dsim::common::entity::old_player_id roster::get_player(const robocup2Dsim::csprotocol::client_id& client) const
{
    auto iter = map_.find(client);
    if (iter != map_.end())
    {
	return iter->second;
    }
    else
    {
	throw std::out_of_range("Client is not in roster");
    }
}

std::string roster::get_team_name(const rce::TeamId& team) const
{
    switch (team)
    {
	case rce::TeamId::ALPHA:
	    return roster_.cbegin()->first;
	default:
	    return roster_.crbegin()->first;
    }
}

enrollment::register_result enrollment::register_client(
	const rcs::client_id& client,
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

enrollment::deregister_result enrollment::deregister_client(const robocup2Dsim::csprotocol::client_id& client)
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
	return enrollment::deregister_result::success;
    }
    else
    {
	return enrollment::deregister_result::client_not_found;
    }
}

bool enrollment::is_registered(const robocup2Dsim::csprotocol::client_id& client) const
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
    bool is_team_full = (enrollment_.size() == ttu::enum_count(rce::TeamId::ALPHA, rce::TeamId::BETA));
    for (auto pair: enrollment_)
    {
	is_team_full &= (pair.second.size() == ttu::enum_count(rce::UniformNumber::ONE, rce::UniformNumber::ELEVEN));
    }
    return is_team_full;
}

} // namespace server
} // namespace robocup2Dsim

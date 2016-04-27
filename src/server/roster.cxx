#include "roster.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>
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
	    if (detail.getTeamName() == get_team_name(rce::team_id::alpha))
	    {
		index_.emplace(client, rce::player_id{ detail.getUniform(), rce::team_id::alpha });
	    }
	    else
	    {
		index_.emplace(client, rce::player_id{ detail.getUniform(), rce::team_id::beta });
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
		    index_.erase(client);
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
    return index_.size() != 0U;
}

roster::finalisation_result roster::finalise()
{
    index_type tmp;
    if (roster_.size() != 2U)
    {
	return finalisation_result::roster_incomplete;
    }
    else
    {
	for (auto id: ttu::enum_iterator<rce::team_id, rce::team_id::alpha, rce::team_id::beta>())
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
		    tmp.emplace(team_roster[uniform], rce::player_id{uniform, id});
		}
	    }
	}
	index_ = std::move(tmp);
	return finalisation_result::success;
    }
}

bool roster::is_registered(const robocup2Dsim::common::entity::player_id& player) const
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
    return index_.find(client) != index_.end();
}

robocup2Dsim::csprotocol::client_id roster::get_client(const robocup2Dsim::common::entity::player_id& player) const
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

robocup2Dsim::common::entity::player_id roster::get_player(const robocup2Dsim::csprotocol::client_id& client) const
{
    auto iter = index_.find(client);
    if (iter != index_.end())
    {
	return iter->second;
    }
    else
    {
	throw std::out_of_range("Client is not in roster");
    }
}

std::string roster::get_team_name(const rce::team_id& team) const
{
    switch (team)
    {
	case rce::team_id::alpha:
	    return roster_.cbegin()->first;
	default:
	    return roster_.crbegin()->first;
    }
}

} // namespace server
} // namespace robocup2Dsim

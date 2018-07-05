#ifndef ROBOCUP2DSIM_SERVER_ROSTER_HPP
#define ROBOCUP2DSIM_SERVER_ROSTER_HPP

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <memory>
#include <tuple>
#include <turbo/type_utility/enum_metadata.hpp>
#include <beam/internet/endpoint.hpp>
#include <robocup2Dsim/common/command.capnp.h>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/csprotocol/protocol.hpp>

namespace robocup2Dsim {
namespace server {

constexpr std::size_t MAX_CLUB_COUNT =  turbo::type_utility::enum_count(
	robocup2Dsim::common::entity::TeamId::ALPHA,
	robocup2Dsim::common::entity::TeamId::BETA);

constexpr std::size_t MAX_TEAM_SIZE = turbo::type_utility::enum_count(
	robocup2Dsim::common::entity::UniformNumber::ONE,
	robocup2Dsim::common::entity::UniformNumber::ELEVEN);

constexpr std::size_t MAX_ROSTER_SIZE = MAX_CLUB_COUNT * MAX_TEAM_SIZE;

enum class deregister_result
{
    success,
    client_not_found
};

class roster
{
public:
    enum find_result
    {
	found,
	not_found
    };
    roster(
	    const std::array<beam::internet::endpoint_id, MAX_ROSTER_SIZE>& enrolled_players,
	    const std::array<std::string, MAX_CLUB_COUNT>& enrolled_teams,
	    const std::array<robocup2Dsim::common::entity::player_id, MAX_CLUB_COUNT> enrolled_goalies);
    inline std::tuple<find_result, beam::internet::endpoint_id> find_client(robocup2Dsim::common::entity::player_id id) const
    {
	return (id < players_.max_size())
		? std::make_tuple(find_result::found, players_[id])
		: std::make_tuple(find_result::not_found, beam::internet::endpoint_id());
    }
    inline std::tuple<find_result, robocup2Dsim::common::entity::player_id> find_player(beam::internet::endpoint_id client) const
    {
	auto iter = std::find(players_.cbegin(), players_.cend(), client);
	return (iter != players_.cend())
		? std::make_tuple(find_result::found, static_cast<robocup2Dsim::common::entity::player_id>(iter - players_.cbegin()))
		: std::make_tuple(find_result::not_found, robocup2Dsim::common::entity::no_player);
    }
    inline bool is_goalkeeper(robocup2Dsim::common::entity::player_id id)
    {
	return std::find(goalies_.cbegin(), goalies_.cend(), id) != goalies_.cend();
    }
    std::string get_team_name(const robocup2Dsim::common::entity::TeamId& team) const;
    deregister_result deregister_client(const beam::internet::endpoint_id& client);
private:
    std::array<beam::internet::endpoint_id, MAX_ROSTER_SIZE> players_;
    std::array<std::string, MAX_CLUB_COUNT> team_names_;
    std::array<robocup2Dsim::common::entity::player_id, MAX_CLUB_COUNT> goalies_;
};

class enrollment
{
public:
    enum class register_result
    {
	success,
	version_mismatch,
	team_slot_taken,
	uniform_taken,
	goalie_taken
    };
    bool is_registered(const beam::internet::endpoint_id& client) const;
    bool is_registered(const std::string& team, robocup2Dsim::common::entity::UniformNumber uniform) const;
    bool is_full() const;
    std::unique_ptr<roster> finalise() const;
    register_result register_client(const beam::internet::endpoint_id& client, const robocup2Dsim::csprotocol::RegistrationRequest::Reader& request);
    deregister_result deregister_client(const beam::internet::endpoint_id& client);
private:
    struct client
    {
	beam::internet::endpoint_id id;
	robocup2Dsim::common::entity::PlayerType ptype;
	inline client(beam::internet::endpoint_id i, robocup2Dsim::common::entity::PlayerType p)
	    :
		id(i), ptype(p)
	{ }
	inline bool operator==(const client& other) const { return id == other.id && ptype == other.ptype; }
    };
    struct player
    {
	std::string team_name;
	robocup2Dsim::common::entity::UniformNumber uniform;
	inline player(const std::string& name, robocup2Dsim::common::entity::UniformNumber number)
	    :
		team_name(name), uniform(number)
	{ }
	inline bool operator==(const player& other) const { return team_name == other.team_name && uniform == other.uniform; }
    };
    typedef std::map<robocup2Dsim::common::entity::UniformNumber, client> team;
    std::map<std::string, team> enrollment_;
    std::map<beam::internet::endpoint_id, player> client_player_map_;
};

} // namespace server
} // namespace robocup2Dsim

#endif

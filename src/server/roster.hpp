#ifndef ROBOCUP2DSIM_SERVER_ROSTER_HPP
#define ROBOCUP2DSIM_SERVER_ROSTER_HPP

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <memory>
#include <tuple>
#include <turbo/type_utility/enum_metadata.hpp>
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

class roster
{
public:
    enum find_result
    {
	found,
	not_found
    };
    roster(
	    const std::array<robocup2Dsim::csprotocol::client_id, MAX_ROSTER_SIZE>& enrolled_players,
	    const std::array<std::string, MAX_CLUB_COUNT>& enrolled_teams,
	    const std::array<robocup2Dsim::common::entity::player_id, MAX_CLUB_COUNT> enrolled_goalies);
    inline std::tuple<find_result, robocup2Dsim::csprotocol::client_id> find_client(robocup2Dsim::common::entity::player_id id) const
    {
	return (id < players_.max_size())
		? std::make_tuple(find_result::found, players_[id])
		: std::make_tuple(find_result::not_found, robocup2Dsim::csprotocol::no_client);
    }
    inline std::tuple<find_result, robocup2Dsim::common::entity::player_id> find_player(robocup2Dsim::csprotocol::client_id client) const
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
    void deregister_client(const robocup2Dsim::csprotocol::client_id& client);
    std::string get_team_name(const robocup2Dsim::common::entity::TeamId& team) const;
private:
    class team
    {
    public:
	typedef std::array<robocup2Dsim::csprotocol::client_id, 11> team_type;
	team();
	robocup2Dsim::csprotocol::client_id& operator[](const robocup2Dsim::common::entity::UniformNumber& uniform);
	const robocup2Dsim::csprotocol::client_id& operator[](const robocup2Dsim::common::entity::UniformNumber& uniform) const;
	inline team_type::iterator begin() { return team_.begin(); }
	inline team_type::iterator end() { return team_.end(); }
	inline team_type::const_iterator cbegin() { return team_.cbegin(); }
	inline team_type::const_iterator cend() { return team_.cend(); }
	bool is_complete() const;
    private:
	team_type team_;
    };
    typedef std::map<std::string, team> roster_type;
    typedef std::map<robocup2Dsim::csprotocol::client_id, robocup2Dsim::common::entity::old_player_id> client_player_map;
    roster_type roster_;
    client_player_map map_;
    std::array<robocup2Dsim::csprotocol::client_id, MAX_ROSTER_SIZE> players_;
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
    enum class deregister_result
    {
	success,
	client_not_found
    };
    register_result register_client(const robocup2Dsim::csprotocol::client_id& client, const robocup2Dsim::csprotocol::RegistrationRequest::Reader& request);
    deregister_result deregister_client(const robocup2Dsim::csprotocol::client_id& client);
    std::unique_ptr<roster> finalise() const;
    bool is_registered(const robocup2Dsim::csprotocol::client_id& client) const;
    bool is_registered(const std::string& team, robocup2Dsim::common::entity::UniformNumber uniform) const;
    bool is_full() const;
private:
    struct client
    {
	robocup2Dsim::csprotocol::client_id id;
	robocup2Dsim::common::entity::PlayerType ptype;
	inline client(robocup2Dsim::csprotocol::client_id i, robocup2Dsim::common::entity::PlayerType p)
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
    std::map<robocup2Dsim::csprotocol::client_id, player> client_player_map_;
};

} // namespace server
} // namespace robocup2Dsim

#endif

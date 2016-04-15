#ifndef ROBOCUP2DSIM_SERVER_ROSTER_HPP
#define ROBOCUP2DSIM_SERVER_ROSTER_HPP

#include <array>
#include <iterator>
#include <map>
#include <robocup2Dsim/common/command.capnp.h>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/entity.hpp>
#include <robocup2Dsim/csprotocol/protocol.hpp>

namespace robocup2Dsim {
namespace server {

class roster
{
public:
    enum class registration_result
    {
	success,
	version_mismatch,
	team_slot_taken,
	team_full,
	uniform_taken,
	goalie_taken
    };
    enum class finalisation_result
    {
	success,
	roster_incomplete
    };
    registration_result register_client(const robocup2Dsim::csprotocol::client_id& client, const robocup2Dsim::csprotocol::RegistrationRequest::Reader& request);
    void deregister_client(const robocup2Dsim::csprotocol::client_id& client);
    bool is_complete() const;
    bool is_finalised() const;
    finalisation_result finalise();
    bool is_registered(const robocup2Dsim::common::entity::player_id& player) const;
    bool is_registered(const robocup2Dsim::csprotocol::client_id& client) const;
    robocup2Dsim::csprotocol::client_id get_client(const robocup2Dsim::common::entity::player_id& player) const;
    robocup2Dsim::common::entity::player_id get_player(const robocup2Dsim::csprotocol::client_id& client) const;
    std::string get_team_name(const robocup2Dsim::common::entity::team_id& team) const;
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
    typedef std::map<robocup2Dsim::csprotocol::client_id, robocup2Dsim::common::entity::player_id> index_type;
    roster_type roster_;
    index_type index_;
};

} // namespace server
} // namespace robocup2Dsim

#endif

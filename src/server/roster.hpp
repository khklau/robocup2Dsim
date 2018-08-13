#ifndef ROBOCUP2DSIM_SERVER_ROSTER_HPP
#define ROBOCUP2DSIM_SERVER_ROSTER_HPP

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <memory>
#include <tuple>
#include <utility>
#include <turbo/type_utility/enum_metadata.hpp>
#include <beam/internet/endpoint.hpp>
#include <robocup2Dsim/common/command.capnp.h>
#include <robocup2Dsim/common/entity.capnp.h>
#include <robocup2Dsim/common/entity.hpp>

namespace robocup2Dsim {
namespace server {

enum class deregister_result
{
    success,
    client_not_found
};

class roster
{
public:
    struct team_sheet
    {
        std::string name;
        robocup2Dsim::common::entity::player_id first_player;
        robocup2Dsim::common::entity::player_id last_player;
    };

    typedef std::array<beam::internet::endpoint_id, common::entity::MAX_ROSTER_SIZE> player_list_type;
    typedef std::array<team_sheet, common::entity::MAX_CLUB_COUNT> team_list_type;
    typedef std::array<robocup2Dsim::common::entity::player_id, common::entity::MAX_CLUB_COUNT> goalie_list_type;

    enum find_result
    {
	found,
	not_found
    };

    class const_iterator : public std::bidirectional_iterator_tag
    {
    public:
        typedef player_list_type::value_type value_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef std::ptrdiff_t difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;

        const_iterator(
                const team_list_type& teams,
                const player_list_type& players,
                player_list_type::const_iterator iterator);

        bool is_valid() const
        {
            return iterator_ != players_->cend();
        }
        robocup2Dsim::common::entity::player_id get_player_id() const;

        inline bool operator==(const const_iterator& other) const
        {
            return this->players_ == other.players_
                    && this->teams_ == other.teams_
                    && this->iterator_ == other.iterator_;
        }
        inline bool operator!=(const const_iterator& other) const
        {
            return !((*this) == other);
        }
        inline const value_type& operator*()
        {
            if (!is_valid())
            {
                throw std::out_of_range("Invalid dereference of a const_iterator outside roster range");
            }
            return *iterator_;
        }
        inline const value_type* operator->()
        {
            return &(*iterator_);
        }
        inline const_iterator operator++()
        {
            ++iterator_;
            return *this;
        }
        inline const_iterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        inline const_iterator operator--()
        {
            return const_iterator(*teams_, *players_, iterator_ == players_->cbegin() ? players_->cend() : --iterator_);
        }
        inline const_iterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

    private:
        const team_list_type* teams_;
        const player_list_type* players_;
        player_list_type::const_iterator iterator_;
    };

    roster(const player_list_type& enrolled_players,
	    const team_list_type& enrolled_teams,
	    const goalie_list_type& enrolled_goalies);

    inline const_iterator cbegin() const
    {
        return const_iterator(teams_, players_, players_.cbegin());
    }

    inline const_iterator cend() const
    {
        return const_iterator(teams_, players_, players_.cend());
    }

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
    player_list_type players_;
    team_list_type teams_;
    goalie_list_type goalies_;
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
    register_result register_client(const beam::internet::endpoint_id& client, const robocup2Dsim::common::command::Registration::Reader& request);
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

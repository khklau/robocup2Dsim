#ifndef ROBOCUP2DSIM_CLIENT_CONFIG_HPP
#define ROBOCUP2DSIM_CLIENT_CONFIG_HPP

#include <cstdint>
#include <string>

namespace robocup2Dsim {
namespace client {

struct config
{
    config() :
	    goalie(false),
	    port(0),
	    bot_arg_count(0),
	    bot_arg_offset(0),
	    bot_msg_queue_length(128),
	    bot_msg_per_sec_limit(16),
	    server_msg_queue_length(128)
    { }
    bool goalie;
    std::string address;
    uint16_t port;
    std::string team;
    uint8_t uniform;
    std::string bot_path;
    uint8_t bot_arg_count;
    uint8_t bot_arg_offset;
    uint16_t bot_msg_queue_length;
    uint16_t bot_msg_per_sec_limit;
    uint16_t server_msg_queue_length;
};

} // namespace client
} // namespace robocup2Dsim

#endif

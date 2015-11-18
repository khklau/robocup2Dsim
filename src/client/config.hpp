#ifndef ROBOCUP2DSIM_CLIENT_CONFIG_HPP
#define ROBOCUP2DSIM_CLIENT_CONFIG_HPP

#include <cstdint>
#include <string>

namespace robocup2dsim {
namespace client {

struct config
{
    config() :
	    goalie(false),
	    port(0),
	    bot_arg_count(0),
	    bot_arg_offset(0)
    { }
    bool goalie;
    std::string address;
    uint16_t port;
    std::string team;
    uint8_t uniform;
    std::string bot_path;
    unsigned short bot_arg_count;
    unsigned short bot_arg_offset;
};

} // namespace client
} // namespace robocup2dsim

#endif

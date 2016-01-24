#ifndef ROBOCUP2DSIM_CLIENT_CONFIG_HPP
#define ROBOCUP2DSIM_CLIENT_CONFIG_HPP

#include <cstdint>
#include <chrono>
#include <string>

namespace robocup2Dsim {
namespace client {

struct config
{
    config() :
	    team("demo"),
	    
	    address("localhost"),
	    port(23230U),
	    goalie(false),
	    bot_arg_count(0),
	    bot_arg_offset(0),
	    bot_msg_queue_length(128U),
	    bot_msg_per_sec_limit(16U),
	    server_msg_queue_length(128U),
	    server_wait_amount(1U),
	    download_bytes_per_sec(0U),
	    upload_bytes_per_sec(0U)
    { }
    std::string team;
    uint8_t uniform;
    std::string address;
    uint16_t port;
    bool goalie;
    std::string bot_path;
    uint8_t bot_arg_count;
    uint8_t bot_arg_offset;
    uint16_t bot_msg_queue_length;
    uint16_t bot_msg_per_sec_limit;
    uint16_t server_msg_queue_length;
    std::chrono::milliseconds server_wait_amount;
    std::size_t download_bytes_per_sec;
    std::size_t upload_bytes_per_sec;
};

} // namespace client
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_CLIENT_CONFIG_HPP
#define ROBOCUP2DSIM_CLIENT_CONFIG_HPP

#include <cstdint>
#include <chrono>
#include <string>
#include <robocup2Dsim/common/entity.capnp.h>

namespace robocup2Dsim {
namespace client {

struct config
{
    config() :
	    team("demo"),
	    uniform(robocup2Dsim::common::entity::UniformNumber::ONE),
	    address("localhost"),
	    port(23230U),
	    goalie(false),
	    bot_arg_count(0),
	    bot_arg_offset(0),
	    bot_msg_queue_length(128U),
	    bot_msg_per_sec_limit(16U),
	    bot_msg_buffer_length(496U),
	    frame_duration(10),
	    simulation_frequency(2),
	    sensor_frequency(2),
	    upload_frequency(2),
	    simulation_start_frame(0),
	    sensor_start_frame(1),
	    upload_start_frame(1),
	    connection_timeout(15000),
	    server_msg_queue_length(128U),
	    server_wait_amount(1U),
	    server_msg_buffer_length(496U),
	    download_bytes_per_sec(0U),
	    upload_bytes_per_sec(0U)
    { }
    std::string team;
    robocup2Dsim::common::entity::UniformNumber uniform;
    std::string address;
    uint16_t port;
    bool goalie;
    std::string bot_path;
    uint8_t bot_arg_count;
    uint8_t bot_arg_offset;
    uint16_t bot_msg_queue_length;
    uint16_t bot_msg_per_sec_limit;
    uint16_t bot_msg_buffer_length;
    std::chrono::milliseconds frame_duration;
    std::size_t simulation_frequency;
    std::size_t sensor_frequency;
    std::size_t upload_frequency;
    std::size_t simulation_start_frame;
    std::size_t sensor_start_frame;
    std::size_t upload_start_frame;
    std::chrono::milliseconds connection_timeout;
    uint16_t server_msg_queue_length;
    std::chrono::milliseconds server_wait_amount;
    uint16_t server_msg_buffer_length;
    std::size_t download_bytes_per_sec;
    std::size_t upload_bytes_per_sec;
};

} // namespace client
} // namespace robocup2Dsim

#endif

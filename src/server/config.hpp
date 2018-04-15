#ifndef ROBOCUP2DSIM_SERVER_CONFIG_HPP
#define ROBOCUP2DSIM_SERVER_CONFIG_HPP

#include <cstdint>
#include <chrono>
#include <string>

namespace robocup2Dsim {
namespace server {

struct config
{
    config() :
	    address("localhost"),
	    port(23230U),
	    ref_arg_count(0),
	    ref_arg_offset(0),
	    ref_msg_queue_length(128U),
	    ref_msg_per_sec_limit(16U),
	    ref_msg_word_length(64U),
	    ref_msg_buffer_capacity(64U),
	    frame_duration(10),
	    simulation_frequency(2),
	    snapshot_frequency(4),
	    simulation_start_frame(0),
	    snapshot_start_frame(1),
	    connection_timeout(15000),
	    client_msg_queue_length(128U),
	    client_wait_amount(1U),
	    client_msg_word_length(124U),
	    client_msg_buffer_capacity(64U),
	    download_bytes_per_sec(0U),
	    upload_bytes_per_sec(0U)
    { }
    std::string address;
    uint16_t port;
    std::string ref_path;
    uint8_t ref_arg_count;
    uint8_t ref_arg_offset;
    uint16_t ref_msg_queue_length;
    uint16_t ref_msg_per_sec_limit;
    uint16_t ref_msg_word_length;
    uint16_t ref_msg_buffer_capacity;
    std::chrono::milliseconds frame_duration;
    std::size_t simulation_frequency;
    std::size_t snapshot_frequency;
    std::size_t simulation_start_frame;
    std::size_t snapshot_start_frame;
    std::chrono::milliseconds connection_timeout;
    uint16_t client_msg_queue_length;
    std::chrono::milliseconds client_wait_amount;
    uint16_t client_msg_word_length;
    uint16_t client_msg_buffer_capacity;
    std::size_t download_bytes_per_sec;
    std::size_t upload_bytes_per_sec;
};

} // namespace server
} // namespace robocup2Dsim

#endif

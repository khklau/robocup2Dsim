#ifndef ROBOCUP2DSIM_SERVER_MONITOR_HPP
#define ROBOCUP2DSIM_SERVER_MONITOR_HPP

#include <beam/internet/endpoint.hpp>

#include <chrono>
#include <deque>
#include <unordered_map>

#include <cstdint>

namespace robocup2Dsim {
namespace server {

struct ping_entry
{
    std::chrono::steady_clock::time_point transmit_time;
    std::uint16_t sequence_num;
};

struct pong_entry
{
    std::chrono::steady_clock::time_point receive_time;
    std::chrono::steady_clock::time_point client_current_time;
    std::uint16_t sequence_num;
};

class clock_monitor
{
public:
    clock_monitor(std::size_t target_sample_size = 20);
    inline std::size_t target_sample_size() const
    {
        return target_sample_size_;
    }
    inline std::size_t transmit_sample_size() const
    {
        return transmitted_pings_.size();
    }
    std::size_t receive_sample_size(beam::internet::endpoint_id client) const;
    std::chrono::steady_clock::duration average_clock_diff(beam::internet::endpoint_id client) const;
    std::uint16_t oldest_receive() const;
    std::uint16_t record_transmit();
    void record_receive(
            beam::internet::endpoint_id client,
            std::uint16_t ping_seq_num,
            std::chrono::steady_clock::time_point client_time);
private:
    std::deque<ping_entry> transmitted_pings_;
    std::unordered_map<beam::internet::endpoint_id, std::deque<pong_entry>> received_pongs_;
    std::size_t target_sample_size_;
    std::uint16_t current_seq_num_;
};

struct monitor
{
    clock_monitor clock;
};

} // namespace server
} // namespace robocup2Dsim

#endif

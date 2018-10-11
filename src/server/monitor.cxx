#include "monitor.hpp"
#include <limits>

namespace robocup2Dsim {
namespace server {

clock_monitor::clock_monitor(std::size_t taret_sample_size)
    :
        transmitted_pings_(),
        received_pongs_(),
        target_sample_size_(taret_sample_size),
        current_seq_num_(0U)
{ }

std::size_t clock_monitor::receive_sample_size(beam::internet::endpoint_id client) const
{
    auto iter = received_pongs_.find(client);
    if (iter == received_pongs_.cend())
    {
        return 0U;
    }
    else
    {
        return iter->second.size();
    }
}

std::chrono::steady_clock::duration clock_monitor::average_clock_diff(beam::internet::endpoint_id client) const
{
    typedef std::chrono::steady_clock::duration duration;
    auto map_iter = received_pongs_.find(client);
    if (map_iter == received_pongs_.cend())
    {
        return duration();
    }
    else if (map_iter->second.size() == 0)
    {
        return duration();
    }
    else
    {
        auto& client_pongs = map_iter->second;
        auto ping_iter = transmitted_pings_.cbegin();
        auto pong_iter = client_pongs.cbegin();
        duration total_clock_diff = std::chrono::milliseconds(0);
        while (ping_iter != transmitted_pings_.cend() && pong_iter != client_pongs.cend())
        {
            if (ping_iter->sequence_num != pong_iter->sequence_num)
            {
                ++ping_iter;
                continue;
            }
            duration round_trip = pong_iter->receive_time - ping_iter->transmit_time;
            // Assuming the round trip took equal time to and from the client
            duration clock_diff = pong_iter->client_current_time
                    - ping_iter->transmit_time
                    - (round_trip / 2);
            total_clock_diff += clock_diff;
            ++ping_iter;
            ++pong_iter;
        }
        return total_clock_diff / static_cast<std::int64_t>(client_pongs.size());
    }
}

std::uint16_t clock_monitor::oldest_receive() const
{
    std::uint16_t oldest_seq = std::numeric_limits<std::uint16_t>::max();
    for (auto& key_value: received_pongs_)
    {
        auto& record = key_value.second;
        if (record.empty())
        {
            continue;
        }
        auto& entry = record.front();
        std::uint16_t current_seq = entry.sequence_num;
        if (current_seq < oldest_seq)
        {
            oldest_seq = current_seq;
        }
    }
    return oldest_seq;
}

std::uint16_t clock_monitor::record_transmit(std::chrono::steady_clock::time_point transmit_time)
{
    ++current_seq_num_;
    transmitted_pings_.push_back({transmit_time, current_seq_num_});
    std::uint16_t oldest_trans = transmitted_pings_.front().sequence_num;
    std::uint16_t oldest_recv = oldest_receive();
    // FIXME: handle sequence number underflow
    while (transmitted_pings_.size() > target_sample_size_ && oldest_trans < oldest_recv)
    {
        transmitted_pings_.pop_front();
        oldest_trans = transmitted_pings_.front().sequence_num;
    }
    return current_seq_num_;
}

void clock_monitor::record_receive(
        beam::internet::endpoint_id client,
        std::uint16_t ping_seq_num,
        std::chrono::steady_clock::time_point client_time,
        std::chrono::steady_clock::time_point receive_time)
{
    received_pongs_[client].push_back({
            receive_time,
            client_time,
            ping_seq_num});
    while (received_pongs_[client].size() > target_sample_size_)
    {
        received_pongs_[client].pop_front();
    }
}

} // namespace server
} // namespace robocup2Dsim

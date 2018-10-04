#include <robocup2Dsim/server/monitor.hpp>

#include <beam/internet/endpoint.hpp>
#include <gtest/gtest.h>

namespace bin = beam::internet;
namespace rse = robocup2Dsim::server;

TEST(clock_monitor_test, record_transmit_basic)
{
    rse::clock_monitor monitor(5);

    std::uint16_t seq1 = monitor.record_transmit();
    std::uint16_t seq2 = monitor.record_transmit();
    std::uint16_t seq3 = monitor.record_transmit();
    std::uint16_t seq4 = monitor.record_transmit();
    std::uint16_t seq5 = monitor.record_transmit();

    EXPECT_EQ(monitor.target_sample_size(), monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(seq1 + 1, seq2) << "The sequence number is not increasing on next transmit";
    EXPECT_EQ(seq2 + 1, seq3) << "The sequence number is not increasing on next transmit";
    EXPECT_EQ(seq3 + 1, seq4) << "The sequence number is not increasing on next transmit";
    EXPECT_EQ(seq4 + 1, seq5) << "The sequence number is not increasing on next transmit";

    monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size(), monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";

    monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size(), monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
}

TEST(clock_monitor_test, record_receive_basic)
{
    const bin::endpoint_id endpoint(99);
    rse::clock_monitor monitor(5);

    monitor.record_receive(endpoint, 1, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, 2, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, 3, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, 4, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, 5, std::chrono::steady_clock::now());

    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint, 6, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(2, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint, 7, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(3, monitor.oldest_receive()) << "The oldest receive calculation is wrong";
}

TEST(clock_monitor_test, needed_sequence_kept_single_client)
{
    const bin::endpoint_id endpoint(99);
    rse::clock_monitor monitor(5);

    std::uint16_t seq1 = monitor.record_transmit();
    std::uint16_t seq2 = monitor.record_transmit();
    std::uint16_t seq3 = monitor.record_transmit();
    std::uint16_t seq4 = monitor.record_transmit();
    std::uint16_t seq5 = monitor.record_transmit();
    monitor.record_receive(endpoint, seq1, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, seq2, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, seq3, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, seq4, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, seq5, std::chrono::steady_clock::now());

    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq6 = monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size() + 1, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq7 = monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint, seq6, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq2, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint, seq7, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq3, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq8 = monitor.record_transmit();
    EXPECT_EQ(seq7 + 1, seq8) << "Wrong next sequence number";
    EXPECT_EQ(monitor.target_sample_size() + 1, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq3, monitor.oldest_receive()) << "The oldest receive calculation is wrong";
}

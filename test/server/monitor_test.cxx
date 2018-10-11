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

TEST(clock_monitor_test, needed_sequence_kept_multiple_clients)
{
    const bin::endpoint_id endpoint1(98);
    const bin::endpoint_id endpoint2(99);
    rse::clock_monitor monitor(5);

    std::uint16_t seq1 = monitor.record_transmit();
    std::uint16_t seq2 = monitor.record_transmit();
    std::uint16_t seq3 = monitor.record_transmit();
    std::uint16_t seq4 = monitor.record_transmit();
    std::uint16_t seq5 = monitor.record_transmit();

    monitor.record_receive(endpoint1, seq1, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint1, seq2, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint1, seq3, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint1, seq4, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint1, seq5, std::chrono::steady_clock::now());

    monitor.record_receive(endpoint2, seq1, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq2, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq3, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq4, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq5, std::chrono::steady_clock::now());

    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq6 = monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size() + 1, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint1, seq6, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 1, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq7 = monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint2, seq6, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq2, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq8 = monitor.record_transmit();
    EXPECT_EQ(seq7 + 1, seq8) << "Wrong next sequence number";
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq2, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint2, seq7, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq2, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq9 = monitor.record_transmit();
    EXPECT_EQ(seq8 + 1, seq9) << "Wrong next sequence number";
    EXPECT_EQ(monitor.target_sample_size() + 3, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq2, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint1, seq7, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 3, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq3, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size() + 3, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq3, monitor.oldest_receive()) << "The oldest receive calculation is wrong";
}

TEST(clock_monitor_test, dropped_ping_single_client)
{
    const bin::endpoint_id endpoint(99);
    rse::clock_monitor monitor(5);

    std::uint16_t seq1 = monitor.record_transmit();
    std::uint16_t seq2 = monitor.record_transmit();
    std::uint16_t seq3 = monitor.record_transmit();
    monitor.record_receive(endpoint, seq1, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, seq3, std::chrono::steady_clock::now());

    EXPECT_EQ(seq2 + 1, seq3) << "Wrong next sequence number";
    EXPECT_EQ(3, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(2, monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq4 = monitor.record_transmit();
    std::uint16_t seq5 = monitor.record_transmit();

    monitor.record_receive(endpoint, seq5, std::chrono::steady_clock::now());
    EXPECT_EQ(seq4 + 1, seq5) << "Wrong next sequence number";
    EXPECT_EQ(5, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(3, monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq6 = monitor.record_transmit();
    std::uint16_t seq7 = monitor.record_transmit();

    monitor.record_receive(endpoint, seq6, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint, seq7, std::chrono::steady_clock::now());
    EXPECT_EQ(7, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq8 = monitor.record_transmit();
    EXPECT_EQ(8, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint, seq8, std::chrono::steady_clock::now());
    EXPECT_EQ(8, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq3, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq9 = monitor.record_transmit();
    EXPECT_EQ(7, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq3, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint, seq9, std::chrono::steady_clock::now());
    EXPECT_EQ(7, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq5, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq10 = monitor.record_transmit();
    EXPECT_EQ(seq9 + 1, seq10) << "Wrong next sequence number";
    EXPECT_EQ(6, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq5, monitor.oldest_receive()) << "The oldest receive calculation is wrong";
}

TEST(clock_monitor_test, dropped_ping_multiple_clients)
{
    const bin::endpoint_id endpoint1(98);
    const bin::endpoint_id endpoint2(99);
    rse::clock_monitor monitor(5);

    std::uint16_t seq1 = monitor.record_transmit();
    std::uint16_t seq2 = monitor.record_transmit();
    std::uint16_t seq3 = monitor.record_transmit();
    std::uint16_t seq4 = monitor.record_transmit();
    std::uint16_t seq5 = monitor.record_transmit();

    monitor.record_receive(endpoint1, seq1, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint1, seq2, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint1, seq5, std::chrono::steady_clock::now());

    monitor.record_receive(endpoint2, seq2, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq3, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq5, std::chrono::steady_clock::now());

    std::uint16_t seq6 = monitor.record_transmit();
    std::uint16_t seq7 = monitor.record_transmit();
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(3, monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(3, monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    monitor.record_receive(endpoint1, seq6, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint1, seq7, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq6, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq7, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 2, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq1, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq8 = monitor.record_transmit();
    monitor.record_receive(endpoint1, seq8, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq8, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 3, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq2, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq9 = monitor.record_transmit();
    monitor.record_receive(endpoint1, seq9, std::chrono::steady_clock::now());
    monitor.record_receive(endpoint2, seq9, std::chrono::steady_clock::now());
    EXPECT_EQ(monitor.target_sample_size() + 3, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq5, monitor.oldest_receive()) << "The oldest receive calculation is wrong";

    std::uint16_t seq10 = monitor.record_transmit();
    EXPECT_EQ(seq9 + 1, seq10) << "Wrong next sequence number";
    EXPECT_EQ(monitor.target_sample_size() + 1, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint1)) << "The number of recorded receives is wrong";
    EXPECT_EQ(monitor.target_sample_size(), monitor.receive_sample_size(endpoint2)) << "The number of recorded receives is wrong";
    EXPECT_EQ(seq5, monitor.oldest_receive()) << "The oldest receive calculation is wrong";
}

TEST(clock_monitor_test, average_clock_diff_basic_server_ahead)
{
    const bin::endpoint_id endpoint(99);
    rse::clock_monitor monitor(4);

    std::chrono::steady_clock::time_point seq1_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point seq2_time = seq1_time + std::chrono::seconds(1);
    std::chrono::steady_clock::time_point seq3_time = seq1_time + std::chrono::seconds(2);
    std::chrono::steady_clock::time_point seq4_time = seq1_time + std::chrono::seconds(3);

    std::uint16_t seq1 = monitor.record_transmit(seq1_time);
    std::uint16_t seq2 = monitor.record_transmit(seq2_time);
    std::uint16_t seq3 = monitor.record_transmit(seq3_time);
    std::uint16_t seq4 = monitor.record_transmit(seq4_time);

    std::chrono::steady_clock::duration single_trip_time = std::chrono::milliseconds(100);

    std::chrono::steady_clock::time_point client_time1 = seq1_time + std::chrono::milliseconds(18);
    std::chrono::steady_clock::time_point client_time2 = seq2_time + std::chrono::milliseconds(19);
    std::chrono::steady_clock::time_point client_time3 = seq3_time + std::chrono::milliseconds(21);
    std::chrono::steady_clock::time_point client_time4 = seq4_time + std::chrono::milliseconds(22);

    monitor.record_receive(endpoint, seq1, client_time1 + single_trip_time, seq1_time + (2 * single_trip_time));
    monitor.record_receive(endpoint, seq2, client_time2 + single_trip_time, seq2_time + (2 * single_trip_time));
    monitor.record_receive(endpoint, seq3, client_time3 + single_trip_time, seq3_time + (2 * single_trip_time));
    monitor.record_receive(endpoint, seq4, client_time4 + single_trip_time, seq4_time + (2 * single_trip_time));

    EXPECT_NE(std::chrono::steady_clock::duration(), monitor.average_clock_diff(endpoint));
    EXPECT_EQ(std::chrono::milliseconds(20), monitor.average_clock_diff(endpoint))
            << "Calculated average clock difference is incorrect";
}

TEST(clock_monitor_test, average_clock_diff_basic_client_ahead)
{
    const bin::endpoint_id endpoint(99);
    rse::clock_monitor monitor(4);

    std::chrono::steady_clock::time_point client_time1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point client_time2 = client_time1 + std::chrono::seconds(1);
    std::chrono::steady_clock::time_point client_time3 = client_time1 + std::chrono::seconds(2);
    std::chrono::steady_clock::time_point client_time4 = client_time1 + std::chrono::seconds(3);

    std::chrono::steady_clock::time_point seq1_time = client_time1 + std::chrono::milliseconds(12);
    std::chrono::steady_clock::time_point seq2_time = client_time2 + std::chrono::milliseconds(11);
    std::chrono::steady_clock::time_point seq3_time = client_time3 + std::chrono::milliseconds(9);
    std::chrono::steady_clock::time_point seq4_time = client_time4 + std::chrono::milliseconds(8);

    std::uint16_t seq1 = monitor.record_transmit(seq1_time);
    std::uint16_t seq2 = monitor.record_transmit(seq2_time);
    std::uint16_t seq3 = monitor.record_transmit(seq3_time);
    std::uint16_t seq4 = monitor.record_transmit(seq4_time);

    std::chrono::steady_clock::duration single_trip_time = std::chrono::milliseconds(100);

    monitor.record_receive(endpoint, seq1, client_time1 + single_trip_time, seq1_time + (2 * single_trip_time));
    monitor.record_receive(endpoint, seq2, client_time2 + single_trip_time, seq2_time + (2 * single_trip_time));
    monitor.record_receive(endpoint, seq3, client_time3 + single_trip_time, seq3_time + (2 * single_trip_time));
    monitor.record_receive(endpoint, seq4, client_time4 + single_trip_time, seq4_time + (2 * single_trip_time));

    EXPECT_NE(std::chrono::steady_clock::duration(), monitor.average_clock_diff(endpoint));
    EXPECT_EQ(std::chrono::milliseconds(-10), monitor.average_clock_diff(endpoint))
            << "Calculated average clock difference is incorrect";
}

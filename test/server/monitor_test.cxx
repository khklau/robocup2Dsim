#include <robocup2Dsim/server/monitor.hpp>
#include <gtest/gtest.h>

namespace rse = robocup2Dsim::server;

TEST(clock_monitor_test, record_transmit_basic)
{
    rse::clock_monitor monitor(5);
    monitor.record_transmit();
    monitor.record_transmit();
    monitor.record_transmit();
    monitor.record_transmit();
    monitor.record_transmit();
    EXPECT_EQ(5, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    monitor.record_transmit();
    EXPECT_EQ(5, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
    monitor.record_transmit();
    EXPECT_EQ(5, monitor.transmit_sample_size()) << "The number of recorded transmits is wrong";
}

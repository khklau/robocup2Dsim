#ifndef ROBOCUP2DSIM_SERVER_PERF_MONITOR_HPP
#define ROBOCUP2DSIM_SERVER_PERF_MONITOR_HPP

#include <chrono>

namespace robocup2Dsim {
namespace server {
namespace perf_monitor {

struct client_stat
{
    std::size_t mtu;
    std::chrono::milliseconds mean_ping;
    float ping_stdev;
};

} // namespace perf_monitor
} // namespace server
} // robocup2Dsim

#endif

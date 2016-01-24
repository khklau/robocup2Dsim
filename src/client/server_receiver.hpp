#ifndef ROBOCUP2DSIM_CLIENT_SERVER_RECEIVER_HPP
#define ROBOCUP2DSIM_CLIENT_SERVER_RECEIVER_HPP

#include <memory>
#include <thread>
#include <asio/io_service.hpp>
#include <beam/message/capnproto.hpp>
#include <beam/queue/unordered_mixed.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include "config.hpp"

namespace robocup2Dsim {
namespace client {

class server_receiver
{
public:
    server_receiver(
	    robocup2Dsim::csprotocol::server_status_queue_type::producer& status_producer,
	    robocup2Dsim::csprotocol::server_trans_queue_type::producer& trans_producer,
	    const config& config);
    ~server_receiver();
    bool running() const { return thread_ != nullptr; }
    void start();
    void stop();
private:
    server_receiver() = delete;
    server_receiver(const server_receiver&) = delete;
    server_receiver& operator=(const server_receiver&) = delete;
    void run();
    robocup2Dsim::csprotocol::server_status_queue_type::producer& status_producer_;
    robocup2Dsim::csprotocol::server_trans_queue_type::producer& trans_producer_;
    std::thread* thread_;
    asio::io_service service_;
    beam::queue::unordered_mixed::receiver<robocup2Dsim::csprotocol::ServerStatus, robocup2Dsim::csprotocol::ServerTransaction> receiver_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

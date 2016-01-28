#ifndef ROBOBUP2DSIM_CLIENT_SERVER_SENDER_HPP
#define ROBOBUP2DSIM_CLIENT_SERVER_SENDER_HPP

#include <thread>
#include <asio/io_service.hpp>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <beam/queue/unordered_mixed.hpp>
#include "config.hpp"

namespace robocup2Dsim {
namespace client {

class server_sender
{
public:
    explicit server_sender(
	    robocup2Dsim::csprotocol::client_status_queue_type::consumer& status_queue,
	    robocup2Dsim::csprotocol::client_trans_queue_type::consumer& trans_queue,
	    const config& config);
    ~server_sender();
    bool running() const { return thread_ != nullptr; }
    void start();
    void stop();
private:
    server_sender() = delete;
    server_sender(const server_sender&) = delete;
    server_sender& operator=(const server_sender&) = delete;
    void run();
    void on_disconnect();
    robocup2Dsim::csprotocol::client_status_queue_type::consumer& status_queue_;
    robocup2Dsim::csprotocol::client_trans_queue_type::consumer& trans_queue_;
    std::thread* thread_;
    asio::io_service service_;
    asio::io_service::strand strand_;
    beam::queue::unordered_mixed::sender<robocup2Dsim::csprotocol::ClientStatus, robocup2Dsim::csprotocol::ClientTransaction> sender_ ;
};

} // namespace client
} // namespace robocup2Dsim

#endif

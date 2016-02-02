#ifndef ROBOCUP2DSIM_CLIENT_SERVER_IO_HPP
#define ROBOCUP2DSIM_CLIENT_SERVER_IO_HPP

#include <thread>
#include <asio/high_resolution_timer.hpp>
#include <asio/io_service.hpp>
#include <asio/strand.hpp>
#include <beam/internet/ipv4.hpp>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include <beam/queue/unordered_mixed.hpp>
#include "config.hpp"

namespace robocup2Dsim {
namespace client {

class server_io
{
public:
    server_io(
	    robocup2Dsim::csprotocol::server_status_queue_type::producer& server_status,
	    robocup2Dsim::csprotocol::server_trans_queue_type::producer& server_trans,
	    robocup2Dsim::csprotocol::client_status_queue_type::consumer& client_status,
	    robocup2Dsim::csprotocol::client_trans_queue_type::consumer& client_trans,
	    const config& config);
    ~server_io();
    bool running() const { return thread_ != nullptr; }
    void start();
    void stop();
private:
    server_io() = delete;
    server_io(const server_io&) = delete;
    server_io& operator=(const server_io&) = delete;
    void run();
    void handle_client_msg();
    void on_connect(const beam::internet::ipv4::address&, const beam::queue::common::port&);
    void on_disconnect(const beam::internet::ipv4::address&, const beam::queue::common::port&);
    void on_receive_server_status(std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerStatus>>);
    void on_receive_server_trans(std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerTransaction>>);
    robocup2Dsim::csprotocol::server_status_queue_type::producer& server_status_;
    robocup2Dsim::csprotocol::server_trans_queue_type::producer& server_trans_;
    robocup2Dsim::csprotocol::client_status_queue_type::consumer& client_status_;
    robocup2Dsim::csprotocol::client_trans_queue_type::consumer& client_trans_;
    std::thread* thread_;
    asio::io_service service_;
    asio::io_service::strand strand_;
    beam::queue::unordered_mixed::receiver<robocup2Dsim::csprotocol::ServerStatus, robocup2Dsim::csprotocol::ServerTransaction> receiver_;
    beam::queue::unordered_mixed::sender<robocup2Dsim::csprotocol::ClientStatus, robocup2Dsim::csprotocol::ClientTransaction> sender_ ;
    decltype(receiver_)::event_handlers receive_handlers_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

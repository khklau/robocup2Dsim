#ifndef ROBOCUP2DSIM_CLIENT_SERVER_IO_HPP
#define ROBOCUP2DSIM_CLIENT_SERVER_IO_HPP

#include <thread>
#include <asio/high_resolution_timer.hpp>
#include <asio/io_service.hpp>
#include <asio/strand.hpp>
#include <beam/internet/ipv4.hpp>
#include <beam/message/capnproto.hpp>
#include <beam/duplex/unordered_mixed.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
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
    typedef beam::duplex::unordered_mixed::in_connection<robocup2Dsim::csprotocol::ServerStatus, robocup2Dsim::csprotocol::ServerTransaction> in_connection_type;
    typedef beam::duplex::unordered_mixed::out_connection<robocup2Dsim::csprotocol::ClientStatus, robocup2Dsim::csprotocol::ClientTransaction> out_connection_type;
    server_io() = delete;
    server_io(const server_io&) = delete;
    server_io& operator=(const server_io&) = delete;
    void run();
    void handle_client_msg(out_connection_type& connection);
    void on_connect(const in_connection_type& connection);
    void on_disconnect(const in_connection_type& connection);
    void on_receive_server_status(const in_connection_type&, std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerStatus>> message);
    void on_receive_server_trans(const in_connection_type&, std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerTransaction>> message);
    robocup2Dsim::csprotocol::server_status_queue_type::producer& server_status_;
    robocup2Dsim::csprotocol::server_trans_queue_type::producer& server_trans_;
    robocup2Dsim::csprotocol::client_status_queue_type::consumer& client_status_;
    robocup2Dsim::csprotocol::client_trans_queue_type::consumer& client_trans_;
    std::thread* thread_;
    asio::io_service service_;
    asio::io_service::strand strand_;
    beam::duplex::unordered_mixed::initiator<in_connection_type, out_connection_type> initiator_ ;
    in_connection_type::event_handlers receive_handlers_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

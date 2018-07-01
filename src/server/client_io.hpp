#ifndef ROBOCUP2DSIM_SERVER_CLIENT_IO_HPP
#define ROBOCUP2DSIM_SERVER_CLIENT_IO_HPP

#include <functional>
#include <thread>
#include <asio/io_service.hpp>
#include <asio/strand.hpp>
#include <beam/duplex/unordered_mixed.hpp>
#include <beam/internet/ipv4.hpp>
#include <beam/message/buffer_pool.hpp>
#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/csprotocol/protocol.capnp.h>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include "config.hpp"

namespace robocup2Dsim {
namespace server {

class client_io
{
public:
    client_io(
	    robocup2Dsim::csprotocol::client_status_queue_type::producer& client_status,
	    robocup2Dsim::csprotocol::client_trans_queue_type::producer& client_trans,
	    robocup2Dsim::csprotocol::server_status_queue_type::consumer& server_status,
	    robocup2Dsim::csprotocol::server_trans_queue_type::consumer& server_trans,
	    const config& config);
    ~client_io();
    bool running() const { return thread_ != nullptr; }
    void start();
    void stop();
private:
    typedef beam::duplex::unordered_mixed::in_connection<robocup2Dsim::csprotocol::ClientStatus, robocup2Dsim::csprotocol::ClientTransaction> in_connection_type;
    typedef beam::duplex::unordered_mixed::out_connection<robocup2Dsim::csprotocol::ServerStatus, robocup2Dsim::csprotocol::ServerTransaction> out_connection_type;
    client_io() = delete;
    client_io(const client_io&) = delete;
    client_io& operator=(const client_io&) = delete;
    void run();
    void handle_server_msg(std::function<out_connection_type*(const beam::internet::ipv4::endpoint_id&)> find);
    void on_connect(const in_connection_type& connection);
    void on_disconnect(const in_connection_type& connection);
    void on_receive_client_status(const in_connection_type&, beam::message::capnproto::payload<robocup2Dsim::csprotocol::ClientStatus>&& message);
    void on_receive_client_trans(const in_connection_type&, beam::message::capnproto::payload<robocup2Dsim::csprotocol::ClientTransaction>&& message);
    config config_;
    beam::message::buffer_pool pool_;
    robocup2Dsim::csprotocol::client_status_queue_type::producer& client_status_;
    robocup2Dsim::csprotocol::client_trans_queue_type::producer& client_trans_;
    robocup2Dsim::csprotocol::server_status_queue_type::consumer& server_status_;
    robocup2Dsim::csprotocol::server_trans_queue_type::consumer& server_trans_;
    std::thread* thread_;
    asio::io_service service_;
    asio::io_service::strand strand_;
    beam::duplex::unordered_mixed::responder<in_connection_type, out_connection_type> responder_;
    in_connection_type::event_handlers receive_handlers_;
};

} // namespace server
} // namespace robocup2Dsim

#endif

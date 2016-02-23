#include "server_io.hpp"
#include <beam/internet/ipv4.hpp>
#include <beam/duplex/common.hpp>
#include <beam/duplex/unordered_mixed.hxx>
#include <glog/logging.h>
#include <turbo/toolset/extension.hpp>
#include <turbo/container/spsc_ring_queue.hxx>

namespace bii4 = beam::internet::ipv4;
namespace bme = beam::message;
namespace bdc = beam::duplex::common;
namespace bdu = beam::duplex::unordered_mixed;
namespace rcs = robocup2Dsim::csprotocol;

namespace robocup2Dsim {
namespace client {

server_io::server_io(
	rcs::server_status_queue_type::producer& server_status,
	rcs::server_trans_queue_type::producer& server_trans,
	rcs::client_status_queue_type::consumer& client_status,
	rcs::client_trans_queue_type::consumer& client_trans,
	const config& config)
    :
	server_status_(server_status),
	server_trans_(server_trans),
	client_status_(client_status),
	client_trans_(client_trans),
	thread_(nullptr),
	service_(),
	strand_(service_),
	initiator_(strand_,
	{
	    1U,
	    config.connection_timeout,
	    config.download_bytes_per_sec,
	    config.upload_bytes_per_sec
	}),
	receive_handlers_(
	{
	    [&](const in_connection_type::event_handlers& current)
	    {
		initiator_.async_receive(current);
	    },
	    std::bind(&server_io::on_connect, this, std::placeholders::_1),
	    std::bind(&server_io::on_disconnect, this, std::placeholders::_1),
	    std::bind(&server_io::on_receive_server_status, this, std::placeholders::_1, std::placeholders::_2),
	    std::bind(&server_io::on_receive_server_trans, this, std::placeholders::_1, std::placeholders::_2)
	})
{
    CHECK(initiator_.connect(bii4::resolve(config.address), config.port) == bdu::connection_result::success) << 
	    "Failed to connect to address " << config.address << " on port " << config.port;
}

server_io::~server_io()
{
    try
    {
	if (initiator_.is_connected())
	{
	    initiator_.disconnect();
	}
        if (!service_.stopped())
        {
            service_.stop();
	}
	if (running())
	{
	    thread_->join();
	    delete thread_;
	    thread_ = nullptr;
	}
    }
    catch (...)
    {
        // do nothing
    }
}

void server_io::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&server_io::run, this));
        thread_ = new std::thread(entry);
    }
}

void server_io::stop()
{
    service_.stop();
}

void server_io::run()
{
    initiator_.async_send(std::bind(&server_io::handle_client_msg, this, std::placeholders::_1));
    initiator_.async_receive(receive_handlers_);
    service_.run();
}

void server_io::handle_client_msg(out_connection_type& connection)
{
    rcs::client_trans_queue_type::consumer::value_type trans_msg;
    while (client_trans_.try_dequeue_move(trans_msg) != rcs::client_trans_queue_type::consumer::result::queue_empty)
    {
	connection.send_reliable(*trans_msg);
    }
    rcs::client_status_queue_type::consumer::value_type status_msg;
    while (client_status_.try_dequeue_move(status_msg) != rcs::client_status_queue_type::consumer::result::queue_empty)
    {
	connection.send_unreliable(*status_msg);
    }
    initiator_.async_send(std::bind(&server_io::handle_client_msg, this, std::placeholders::_1));
}

void server_io::on_connect(const in_connection_type&)
{
    // TODO should validate that the connection is from the server we expect?
    initiator_.async_receive(receive_handlers_);
}

void server_io::on_disconnect(const in_connection_type&)
{
    // TODO should validate that the disconnection is from the server we expect?
    std::unique_ptr<bme::capnproto<rcs::ServerTransaction>> message(new bme::capnproto<rcs::ServerTransaction>());
    message->get_builder().setBye();
    if (TURBO_UNLIKELY(server_trans_.try_enqueue_move(std::move(message)) != rcs::server_trans_queue_type::producer::result::success))
    {
	LOG(WARNING) << "server transaction queue is full; dropping disconnect error";
    }
    stop();
}

void server_io::on_receive_server_status(const in_connection_type&, std::unique_ptr<bme::capnproto<rcs::ServerStatus>> message)
{
    if (TURBO_UNLIKELY(server_status_.try_enqueue_move(std::move(message)) != rcs::server_status_queue_type::producer::result::success))
    {
	LOG(WARNING) << "server status queue is full; dropping message";
    }
    initiator_.async_receive(receive_handlers_);
}

void server_io::on_receive_server_trans(const in_connection_type&, std::unique_ptr<bme::capnproto<rcs::ServerTransaction>> message)
{
    if (TURBO_UNLIKELY(server_trans_.try_enqueue_move(std::move(message)) != rcs::server_trans_queue_type::producer::result::success))
    {
	LOG(WARNING) << "server transaction queue is full; dropping message";
    }
    initiator_.async_receive(receive_handlers_);
}

} // namespace client
} // namespace robocup2Dsim

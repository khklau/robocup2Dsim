#include "client_io.hpp"
#include <array>
#include <beam/internet/ipv4.hpp>
#include <beam/duplex/common.hpp>
#include <beam/duplex/unordered_mixed.hxx>
#include <glog/logging.h>
#include <robocup2Dsim/common/entity.capnp.h>
#include <turbo/toolset/extension.hpp>
#include <turbo/container/spsc_ring_queue.hxx>

namespace bii4 = beam::internet::ipv4;
namespace bme = beam::message;
namespace bdc = beam::duplex::common;
namespace bdu = beam::duplex::unordered_mixed;
namespace rcc = robocup2Dsim::common;
namespace rcs = robocup2Dsim::csprotocol;

namespace robocup2Dsim {
namespace server {

client_io::client_io(
	rcs::client_status_queue_type::producer& client_status,
	rcs::client_trans_queue_type::producer& client_trans,
	rcs::server_status_queue_type::consumer& server_status,
	rcs::server_trans_queue_type::consumer& server_trans,
	const config& config)
    :
	client_status_(client_status),
	client_trans_(client_trans),
	server_status_(server_status),
	server_trans_(server_trans),
	thread_(nullptr),
	service_(),
	strand_(service_),
	responder_(strand_,
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
		responder_.async_receive(current);
	    },
	    std::bind(&client_io::on_connect, this, std::placeholders::_1),
	    std::bind(&client_io::on_disconnect, this, std::placeholders::_1),
	    std::bind(&client_io::on_receive_client_status, this, std::placeholders::_1, std::placeholders::_2),
	    std::bind(&client_io::on_receive_client_trans, this, std::placeholders::_1, std::placeholders::_2)
	})
{
    CHECK(responder_.bind({0U, config.port}) == bdu::bind_result::success) << "Failed to bind to port " << config.port;
}

client_io::~client_io()
{
    try
    {
	if (responder_.is_bound())
	{
	    responder_.unbind();
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

void client_io::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&client_io::run, this));
        thread_ = new std::thread(entry);
    }
}

void client_io::stop()
{
    service_.stop();
}

void client_io::run()
{
    responder_.async_send(std::bind(&client_io::handle_server_msg, this, std::placeholders::_1));
    responder_.async_receive(receive_handlers_);
    service_.run();
}

void client_io::handle_server_msg(std::function<out_connection_type*(const beam::duplex::common::endpoint_id&)> find)
{
    rcs::server_trans_queue_type::consumer::value_type trans_msg;
    while (server_trans_.try_dequeue_move(trans_msg) != rcs::server_trans_queue_type::consumer::result::queue_empty)
    {
	out_connection_type* connection = find(rcs::convert(trans_msg->get_reader().getRecipient()));
	if (TURBO_LIKELY(connection != nullptr))
	{
	    connection->send_reliable(*trans_msg);
	}
	else
	{
	    LOG(WARNING) << "Server requested transaction message to be sent to a client that is no longer connected";
	}
    }
    rcs::server_status_queue_type::consumer::value_type status_msg;
    while (server_status_.try_dequeue_move(status_msg) != rcs::server_status_queue_type::consumer::result::queue_empty)
    {
	out_connection_type* connection = find(rcs::convert(status_msg->get_reader().getRecipient()));
	if (TURBO_LIKELY(connection != nullptr))
	{
	    connection->send_unreliable(*status_msg);
	}
	else
	{
	    LOG(WARNING) << "Server requested status message to be sent to a client that is no longer connected";
	}
    }
    responder_.async_send(std::bind(&client_io::handle_server_msg, this, std::placeholders::_1));
}

void client_io::on_connect(const in_connection_type&)
{
    // TODO should validate that the connection is from the client we expect?
    responder_.async_receive(receive_handlers_);
}

void client_io::on_disconnect(const in_connection_type&)
{
    // TODO should validate that the disconnection is from the client we expect?
    std::unique_ptr<bme::capnproto<rcs::ClientTransaction>> message(new bme::capnproto<rcs::ClientTransaction>());
    message->get_builder().setDisconnect();
    if (TURBO_UNLIKELY(client_trans_.try_enqueue_move(std::move(message)) != rcs::client_trans_queue_type::producer::result::success))
    {
	LOG(WARNING) << "client transaction queue is full; dropping disconnect error";
    }
}

void client_io::on_receive_client_status(const in_connection_type&, std::unique_ptr<bme::capnproto<rcs::ClientStatus>> message)
{
    if (TURBO_UNLIKELY(client_status_.try_enqueue_move(std::move(message)) != rcs::client_status_queue_type::producer::result::success))
    {
	LOG(WARNING) << "client status queue is full; dropping message";
    }
    responder_.async_receive(receive_handlers_);
}

void client_io::on_receive_client_trans(const in_connection_type&, std::unique_ptr<bme::capnproto<rcs::ClientTransaction>> message)
{
    if (TURBO_UNLIKELY(client_trans_.try_enqueue_move(std::move(message)) != rcs::client_trans_queue_type::producer::result::success))
    {
	LOG(WARNING) << "client transaction queue is full; dropping message";
    }
    responder_.async_receive(receive_handlers_);
}

} // namespace server
} // namespace robocup2Dsim

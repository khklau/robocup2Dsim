#include "server_io.hpp"
#include <beam/internet/ipv4.hpp>
#include <beam/queue/common.hpp>
#include <beam/queue/unordered_mixed.hxx>
#include <glog/logging.h>
#include <turbo/toolset/extension.hpp>
#include <turbo/container/spsc_ring_queue.hxx>

namespace bii4 = beam::internet::ipv4;
namespace bme = beam::message;
namespace bqc = beam::queue::common;
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
	receiver_(strand_, { 1U, config.server_wait_amount, config.download_bytes_per_sec, config.upload_bytes_per_sec }),
	sender_(strand_,
	{
	    std::bind(&server_io::on_disconnect, this, std::placeholders::_1, std::placeholders::_2)
	},
	{
	    config.server_msg_queue_length,
	    config.server_wait_amount,
	    config.connection_timeout,
	    config.download_bytes_per_sec,
	    config.upload_bytes_per_sec
	}),
	receive_handlers_(
	{
	    [&](const decltype(receiver_)::event_handlers& current)
	    {
		receiver_.async_receive(current);
	    },
	    std::bind(&server_io::on_connect, this, std::placeholders::_1, std::placeholders::_2),
	    std::bind(&server_io::on_disconnect, this, std::placeholders::_1, std::placeholders::_2),
	    std::bind(&server_io::on_receive_server_status, this, std::placeholders::_1),
	    std::bind(&server_io::on_receive_server_trans, this, std::placeholders::_1)
	})
{
    CHECK(receiver_.bind({ 0U, config.port }) == decltype(receiver_)::bind_result::success) << "Failed to bind to port " << config.port;
    CHECK(sender_.connect(bii4::resolve(config.address), config.port) == decltype(sender_)::connection_result::success) << 
	    "Failed to connect to address " << config.address << " on port " << config.port;
}

server_io::~server_io()
{
    try
    {
	if (sender_.is_connected())
	{
	    sender_.disconnect();
	}
	if (receiver_.is_bound())
	{
	    receiver_.unbind();
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
    service_.post(std::bind(&server_io::handle_client_msg, this));
    receiver_.async_receive(receive_handlers_);
    service_.run();
}

void server_io::handle_client_msg()
{
    rcs::client_trans_queue_type::consumer::value_type trans_msg;
    while (client_trans_.try_dequeue_move(trans_msg) != rcs::client_trans_queue_type::consumer::result::queue_empty)
    {
	if (TURBO_UNLIKELY(sender_.send_reliable(*trans_msg) != decltype(sender_)::send_result::success))
	{
	    LOG(WARNING) << "failed to send client transaction; dropping transaction";
	}
    }
    rcs::client_status_queue_type::consumer::value_type status_msg;
    while (client_status_.try_dequeue_move(status_msg) != rcs::client_status_queue_type::consumer::result::queue_empty)
    {
	if (TURBO_UNLIKELY(sender_.send_unreliable(*status_msg) != decltype(sender_)::send_result::success))
	{
	    LOG(WARNING) << "failed to send client status; dropping status";
	}
    }
    service_.post(std::bind(&server_io::handle_client_msg, this));
}

void server_io::on_connect(const bii4::address&, const bqc::port&)
{
    // TODO should validate that the connection is from the server we expect?
    receiver_.async_receive(receive_handlers_);
}

void server_io::on_disconnect(const bii4::address&, const bqc::port&)
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

void server_io::on_receive_server_status(std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerStatus>> message)
{
    if (TURBO_UNLIKELY(server_status_.try_enqueue_move(std::move(message)) != rcs::server_status_queue_type::producer::result::success))
    {
	LOG(WARNING) << "server status queue is full; dropping message";
    }
    receiver_.async_receive(receive_handlers_);
}

void server_io::on_receive_server_trans(std::unique_ptr<beam::message::capnproto<robocup2Dsim::csprotocol::ServerTransaction>> message)
{
    if (TURBO_UNLIKELY(server_trans_.try_enqueue_move(std::move(message)) != rcs::server_trans_queue_type::producer::result::success))
    {
	LOG(WARNING) << "server transaction queue is full; dropping message";
    }
    receiver_.async_receive(receive_handlers_);
}

} // namespace client
} // namespace robocup2Dsim

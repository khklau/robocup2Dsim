#include "server_receiver.hpp"
#include <beam/internet/ipv4.hpp>
#include <beam/message/capnproto.hpp>
#include <beam/queue/common.hpp>
#include <beam/queue/unordered_mixed.hxx>
#include <glog/logging.h>
#include <turbo/container/spsc_ring_queue.hxx>
#include <turbo/toolset/extension.hpp>

namespace bii4 = beam::internet::ipv4;
namespace bme = beam::message;
namespace bqc = beam::queue::common;
namespace rcs = robocup2Dsim::csprotocol;

namespace robocup2Dsim {
namespace client {

server_receiver::server_receiver(rcs::server_status_queue_type::producer& status_producer, rcs::server_trans_queue_type::producer& trans_producer, const config& config) :
	status_producer_(status_producer),
	trans_producer_(trans_producer),
	thread_(nullptr),
	service_(),
	receiver_(service_, { 1U, config.server_wait_amount, config.download_bytes_per_sec, config.upload_bytes_per_sec })
{
    CHECK(receiver_.bind({ 0U, config.port }) == decltype(receiver_)::bind_result::success) << "Failed to bind to port " << config.port;
}

server_receiver::~server_receiver()
{
    try
    {
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

void server_receiver::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&server_receiver::run, this));
        thread_ = new std::thread(entry);
    }
}

void server_receiver::stop()
{
    service_.stop();
}

void server_receiver::run()
{
    decltype(receiver_)::event_handlers handlers
    {
	[&](const decltype(receiver_)::event_handlers& current)
	{
	    receiver_.async_receive(current);
	},
	[&](const bii4::address& address, const bqc::port&)
	{
	    // TODO should validate that the connection is from the server we expect?
	    receiver_.async_receive(handlers);
	},
	[&](const bii4::address&, const bqc::port&)
	{
	    stop();
	},
	[&](std::unique_ptr<bme::capnproto<rcs::ServerStatus>> message)
	{
	    if (status_producer_.try_enqueue_move(std::move(message)) != rcs::server_status_queue_type::producer::result::success)
	    {
		LOG(WARNING) << "server status queue is full; dropping message";
	    }
	    receiver_.async_receive(handlers);
	},
	[&](std::unique_ptr<bme::capnproto<rcs::ServerTransaction>> message)
	{
	    if (trans_producer_.try_enqueue_move(std::move(message)) != rcs::server_trans_queue_type::producer::result::success)
	    {
		LOG(WARNING) << "server transaction queue is full; dropping message";
	    }
	    receiver_.async_receive(handlers);
	}
    };
    receiver_.async_receive(handlers);
    service_.run();
}

} // namespace client
} // namespace robocup2Dsim

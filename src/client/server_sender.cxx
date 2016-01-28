#include "server_sender.hpp"
#include <beam/internet/ipv4.hpp>
#include <beam/message/capnproto.hpp>
#include <beam/queue/common.hpp>
#include <beam/queue/unordered_mixed.hxx>
#include <glog/logging.h>
#include <turbo/container/spsc_ring_queue.hxx>

namespace bii4 = beam::internet::ipv4;
namespace bme = beam::message;
namespace rcs = robocup2Dsim::csprotocol;

namespace robocup2Dsim {
namespace client {

server_sender::server_sender(rcs::client_status_queue_type::consumer& status_queue, rcs::client_trans_queue_type::consumer& trans_queue, const config& config) :
	status_queue_(status_queue),
	trans_queue_(trans_queue),
	thread_(nullptr),
	service_(),
	strand_(service_),
	sender_(strand_,
	{
	    std::bind(&server_sender::on_disconnect, this)
	},
	{
	    config.server_msg_queue_length,
	    config.server_wait_amount,
	    config.connection_timeout,
	    config.download_bytes_per_sec,
	    config.upload_bytes_per_sec
	})
{
    CHECK(sender_.connect(bii4::resolve(config.address), config.port) == decltype(sender_)::connection_result::success) << 
	    "Failed to connect to address " << config.address << " on port " << config.port;
}

server_sender::~server_sender()
{
    try
    {
	if (sender_.is_connected())
	{
	    sender_.disconnect();
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

void server_sender::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&server_sender::run, this));
        thread_ = new std::thread(entry);
    }
}

void server_sender::stop()
{
    service_.stop();
}

void server_sender::run()
{
    // TODO fix accessing values stored on another thread's stack 
}

void server_sender::on_disconnect()
{
    stop();
}

} // namespace client
} // namespace robocup2Dsim

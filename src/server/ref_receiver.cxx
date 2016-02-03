#include "ref_receiver.hpp"
#include <utility>
#include <beam/message/capnproto.hxx>
#include <glog/logging.h>
#include <kj/io.h>
#include <turbo/toolset/extension.hpp>
#include <turbo/container/spsc_ring_queue.hxx>

namespace bme = beam::message;
namespace rsr = robocup2Dsim::srprotocol;
namespace tip = turbo::ipc::posix;

namespace robocup2Dsim {
namespace server {

ref_receiver::ref_receiver(tip::pipe::front&& ref_stdout, rsr::ref_output_queue_type::producer& producer) :
	ref_stdout_(std::move(ref_stdout)),
	producer_(producer),
	thread_(nullptr),
	service_(),
	stream_(service_, ref_stdout_.get_handle())
{ }

ref_receiver::~ref_receiver()
{
    try
    {
        if (!service_.stopped())
        {
            service_.stop();
            if (running())
            {
                thread_->join();
                delete thread_;
            }
        }
    }
    catch (...)
    {
        // do nothing
    }
}

void ref_receiver::reset()
{
    std::function<void (const asio::error_code&, std::size_t)> receiver = std::bind(
	    &ref_receiver::receive,
	    this,
	    std::placeholders::_1,
	    std::placeholders::_2);
    stream_.async_read_some(asio::null_buffers(), receiver);
}

void ref_receiver::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&ref_receiver::run, this));
        thread_ = new std::thread(entry);
    }
}

void ref_receiver::stop()
{
    service_.stop();
}

void ref_receiver::run()
{
    reset();
    service_.run();
}

void ref_receiver::receive(const asio::error_code& error, std::size_t bytes_received)
{
    rsr::ref_output_queue_type::producer::result result = rsr::ref_output_queue_type::producer::result::success;
    // More than 1 message may be available, so we need to consume all of them
    while (!error.value() && bytes_received > 0 && ref_stdout_.available() > 0)
    {
	kj::FdInputStream input(ref_stdout_.get_handle());
	std::unique_ptr<bme::capnproto<rsr::RefOutput>> message(new bme::capnproto<rsr::RefOutput>(input));
	result = producer_.try_enqueue_move(std::move(message));
	if (TURBO_UNLIKELY(result != rsr::ref_output_queue_type::producer::result::success))
	{
	    LOG(WARNING) << "ref processing queue is full: message dropped!";
	}
    }
    reset();
}

} // namespace server
} // namespace robocup2Dsim

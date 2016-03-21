#include "ref_io.hpp"
#include <utility>
#include <beam/message/capnproto.hxx>
#include <capnp/serialize.h>
#include <glog/logging.h>
#include <kj/io.h>
#include <turbo/toolset/extension.hpp>
#include <turbo/container/spsc_ring_queue.hxx>

namespace bme = beam::message;
namespace rsr = robocup2Dsim::srprotocol;
namespace tip = turbo::ipc::posix;

namespace robocup2Dsim {
namespace server {

ref_io::ref_io(tip::pipe::front& ref_stdout, tip::pipe::back& ref_stdin, rsr::ref_output_queue_type::producer& producer, rsr::ref_input_queue_type::consumer& consumer) :
	ref_stdout_(ref_stdout),
	ref_stdin_(ref_stdin),
	producer_(producer),
	consumer_(consumer),
	thread_(nullptr),
	service_(),
	stream_(service_, ref_stdout_.get_handle())
{ }

ref_io::~ref_io()
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

void ref_io::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&ref_io::run, this));
        thread_ = new std::thread(entry);
    }
}

void ref_io::stop()
{
    service_.stop();
}

void ref_io::run()
{
    service_.post(std::bind(&ref_io::send, this));
    stream_.async_read_some(asio::null_buffers(), std::bind(
	    &ref_io::receive,
	    this,
	    std::placeholders::_1,
	    std::placeholders::_2));
    service_.run();
}

void ref_io::receive(const asio::error_code& error, std::size_t bytes_received)
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
    stream_.async_read_some(asio::null_buffers(), std::bind(
	    &ref_io::receive,
	    this,
	    std::placeholders::_1,
	    std::placeholders::_2));
}

void ref_io::send()
{
    rsr::ref_input_queue_type::consumer::value_type message;
    while (consumer_.try_dequeue_move(message) != rsr::ref_input_queue_type::consumer::result::queue_empty)
    {
	capnp::writeMessageToFd(ref_stdin_.get_handle(), message->get_segments());
    }
    service_.post(std::bind(&ref_io::send, this));
}

} // namespace server
} // namespace robocup2Dsim

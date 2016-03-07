#include "bot_io.hpp"
#include <utility>
#include <beam/message/capnproto.hxx>
#include <glog/logging.h>
#include <kj/io.h>
#include <turbo/toolset/extension.hpp>
#include <turbo/container/spsc_ring_queue.hxx>

namespace bme = beam::message;
namespace rbc = robocup2Dsim::bcprotocol;
namespace tip = turbo::ipc::posix;

namespace robocup2Dsim {
namespace client {

bot_io::bot_io(
	tip::pipe::back& bot_stdin,
	tip::pipe::front& bot_stdout,
	rbc::bot_input_queue_type::consumer& consumer,
	rbc::bot_output_queue_type::producer& producer)
    :
	bot_stdin_(bot_stdin),
	bot_stdout_(bot_stdout),
	consumer_(consumer),
	producer_(producer),
	thread_(nullptr),
	service_(),
	stream_(service_, bot_stdout_.get_handle())
{ }

bot_io::~bot_io()
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

void bot_io::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&bot_io::run, this));
        thread_ = new std::thread(entry);
    }
}

void bot_io::stop()
{
    service_.stop();
}

void bot_io::run()
{
    service_.post(std::bind(&bot_io::send, this));
    stream_.async_read_some(asio::null_buffers(), std::bind(
	    &bot_io::receive,
	    this,
	    std::placeholders::_1,
	    std::placeholders::_2));
    service_.run();
}

void bot_io::send()
{
    rbc::bot_input_queue_type::consumer::value_type message;
    while (consumer_.try_dequeue_move(message) != rbc::bot_input_queue_type::consumer::result::queue_empty)
    {
	capnp::writeMessageToFd(bot_stdin_.get_handle(), message->get_segments());
    }
    service_.post(std::bind(&bot_io::send, this));
}

void bot_io::receive(const asio::error_code& error, std::size_t bytes_received)
{
    rbc::bot_output_queue_type::producer::result result = rbc::bot_output_queue_type::producer::result::success;
    // More than 1 message may be available, so we need to consume all of them
    while (!error.value() && bytes_received > 0 && bot_stdout_.available() > 0)
    {
	kj::FdInputStream input(bot_stdout_.get_handle());
	std::unique_ptr<bme::capnproto<rbc::BotOutput>> message(new bme::capnproto<rbc::BotOutput>(input));
	result = producer_.try_enqueue_move(std::move(message));
	if (TURBO_UNLIKELY(result != rbc::bot_output_queue_type::producer::result::success))
	{
	    LOG(WARNING) << "bot processing queue is full: message dropped!";
	}
    }
    stream_.async_read_some(asio::null_buffers(), std::bind(
	    &bot_io::receive,
	    this,
	    std::placeholders::_1,
	    std::placeholders::_2));
}

} // namespace client
} // namespace robocup2Dsim

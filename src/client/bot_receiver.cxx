#include "bot_receiver.hpp"
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

bot_receiver::bot_receiver(tip::pipe::front&& bot_stdout, bot_out_queue_type::producer& producer) :
	bot_stdout_(std::move(bot_stdout)),
	producer_(producer),
	thread_(nullptr),
	service_(),
	stream_(service_, bot_stdout_.get_handle())
{ }

bot_receiver::~bot_receiver()
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

void bot_receiver::reset()
{
    std::function<void (const asio::error_code&, std::size_t)> receiver = std::bind(
	    &bot_receiver::receive,
	    this,
	    std::placeholders::_1,
	    std::placeholders::_2);
    stream_.async_read_some(asio::null_buffers(), receiver);
}

void bot_receiver::start()
{
    if (!running())
    {
        std::function<void ()> entry(std::bind(&bot_receiver::run, this));
        thread_ = new std::thread(entry);
    }
}

void bot_receiver::stop()
{
    service_.stop();
}

void bot_receiver::run()
{
    reset();
    service_.run();
}

void bot_receiver::receive(const asio::error_code& error, std::size_t bytes_received)
{
    bot_out_queue_type::producer::result result = bot_out_queue_type::producer::result::success;
    // More than 1 message may be available, so we need to consume all of them
    while (!error.value() && bytes_received > 0 && bot_stdout_.available() > 0)
    {
	kj::FdInputStream input(bot_stdout_.get_handle());
	std::unique_ptr<bme::capnproto<rbc::BotTransmission>> message(new bme::capnproto<rbc::BotTransmission>(input));
	result = producer_.try_enqueue_move(std::move(message));
	if (TURBO_UNLIKELY(result != bot_out_queue_type::producer::result::success))
	{
	    LOG(WARNING) << "bot processing queue is full: message dropped!";
	}
    }
    reset();
}

} // namespace client
} // namespace robocup2Dsim

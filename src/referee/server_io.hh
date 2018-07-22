#ifndef ROBOCUP2DSIM_REFEREE_SERVER_IO_HXX
#define ROBOCUP2DSIM_REFEREE_SERVER_IO_HXX

#include "server_io.hpp"
#include <functional>

namespace robocup2Dsim {
namespace referee {

namespace bmc = beam::message::capnproto;
namespace rre = robocup2Dsim::referee;
namespace rsr = robocup2Dsim::srprotocol;

template <class read_func_type>
void server_io::run(read_func_type&& func)
{
    async_read(std::forward<read_func_type>(func));
    service_.run();
}

template <class read_func_type>
void server_io::async_read(read_func_type&& read_func)
{
    std::function<void(const asio::error_code&, std::size_t)> callback = 
    [this, &callback, &read_func] (const asio::error_code& error, std::size_t bytes_received)
    {
	asio::posix::descriptor_base::bytes_readable command(true);
	stdin_.io_control(command);
	while (!error.value() && bytes_received > 0 && command.get() > 0)
	{
	    input_queue_.push(std::move(bmc::read<rsr::RefInput>(
		    stdin_.native_handle(),
		    config_.msg_word_length,
		    pool_)));
	}
	read_func(input_queue_, output_queue_);
	stdin_.async_read_some(asio::null_buffers(), callback);
    };
    stdin_.async_read_some(asio::null_buffers(), callback);
}

} // namespace referee
} // namespace robocup2Dsim

#endif

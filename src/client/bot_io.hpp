#ifndef ROBOCUP2DSIM_CLIENT_BOT_IO_HPP
#define ROBOCUP2DSIM_CLIENT_BOT_IO_HPP

#include <memory>
#include <thread>
#include <asio/error_code.hpp>
#include <asio/io_service.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <beam/message/buffer_pool.hpp>
#include <beam/message/capnproto.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/ipc/posix/pipe.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include "config.hpp"

namespace robocup2Dsim {
namespace client {

class bot_io
{
public:
    bot_io(
	    const config& conf,
	    turbo::ipc::posix::pipe::back& bot_stdin,
	    turbo::ipc::posix::pipe::front& bot_stdout,
	    robocup2Dsim::bcprotocol::bot_input_queue_type::consumer& consumer,
	    robocup2Dsim::bcprotocol::bot_output_queue_type::producer& producer);
    ~bot_io();
    bool running() const { return thread_ != nullptr; }
    void start();
    void stop();
private:
    bot_io() = delete;
    bot_io(const bot_io& other) = delete;
    bot_io& operator=(const bot_io& other) = delete;
    void run();
    void send();
    void receive(const asio::error_code& error, std::size_t bytes_received);
    config config_;
    beam::message::buffer_pool pool_;
    turbo::ipc::posix::pipe::back& bot_stdin_;
    turbo::ipc::posix::pipe::front& bot_stdout_;
    robocup2Dsim::bcprotocol::bot_input_queue_type::consumer& consumer_;
    robocup2Dsim::bcprotocol::bot_output_queue_type::producer& producer_;
    std::thread* thread_;
    asio::io_service service_;
    asio::posix::stream_descriptor stream_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

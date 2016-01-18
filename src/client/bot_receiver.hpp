#ifndef ROBOCUP2DSIM_CLIENT_BOT_RECEIVER_HPP
#define ROBOCUP2DSIM_CLIENT_BOT_RECEIVER_HPP

#include <memory>
#include <thread>
#include <asio/error_code.hpp>
#include <asio/io_service.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <beam/message/capnproto.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/ipc/posix/pipe.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include "specification.hpp"

namespace robocup2Dsim {
namespace client {

class bot_receiver
{
public:
    bot_receiver(turbo::ipc::posix::pipe::front&& bot_stdout, robocup2Dsim::bcprotocol::bot_trans_queue_type::producer& producer);
    ~bot_receiver();
    bool running() const { return thread_ != nullptr; }
    void reset();
    void start();
    void stop();
private:
    bot_receiver(const bot_receiver& other) = delete;
    bot_receiver& operator=(const bot_receiver& other) = delete;
    void run();
    void receive(const asio::error_code& error, std::size_t bytes_received);
    turbo::ipc::posix::pipe::front bot_stdout_;
    robocup2Dsim::bcprotocol::bot_trans_queue_type::producer& producer_;
    std::thread* thread_;
    asio::io_service service_;
    asio::posix::stream_descriptor stream_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

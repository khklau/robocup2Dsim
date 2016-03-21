#ifndef ROBOCUP2DSIM_SERVER_REF_IO_HPP
#define ROBOCUP2DSIM_SERVER_REF_IO_HPP

#include <memory>
#include <thread>
#include <asio/error_code.hpp>
#include <asio/io_service.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <beam/message/capnproto.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/ipc/posix/pipe.hpp>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <robocup2Dsim/srprotocol/protocol.hpp>

namespace robocup2Dsim {
namespace server {

class ref_io
{
public:
    ref_io(
	    turbo::ipc::posix::pipe::front& ref_stdout,
	    turbo::ipc::posix::pipe::back& ref_stdin,
	    robocup2Dsim::srprotocol::ref_output_queue_type::producer& producer,
	    robocup2Dsim::srprotocol::ref_input_queue_type::consumer& consumer);
    ~ref_io();
    bool running() const { return thread_ != nullptr; }
    void start();
    void stop();
private:
    ref_io() = delete;
    ref_io(const ref_io& other) = delete;
    ref_io& operator=(const ref_io& other) = delete;
    void run();
    void send();
    void receive(const asio::error_code& error, std::size_t bytes_received);
    turbo::ipc::posix::pipe::front& ref_stdout_;
    turbo::ipc::posix::pipe::back& ref_stdin_;
    robocup2Dsim::srprotocol::ref_output_queue_type::producer& producer_;
    robocup2Dsim::srprotocol::ref_input_queue_type::consumer& consumer_;
    std::thread* thread_;
    asio::io_service service_;
    asio::posix::stream_descriptor stream_;
};

} // namespace server
} // namespace robocup2Dsim

#endif

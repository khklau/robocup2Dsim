#ifndef ROBOCUP2DSIM_REFEREE_SERVER_IO_HPP
#define ROBOCUP2DSIM_REFEREE_SERVER_IO_HPP

#include <queue>
#include <asio/io_service.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <beam/message/buffer_pool.hpp>
#include <beam/message/capnproto.hh>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <robocup2Dsim/srprotocol/protocol.hpp>
#include "config.hpp"

namespace robocup2Dsim {
namespace referee {

class server_io
{
public:
    server_io(const config& conf);
    ~server_io();
    inline void stop()
    {
	service_.stop();
    }
    template <class read_func_type>
    void run(read_func_type&& func);
private:
    template <class read_func_type>
    void async_read(read_func_type&& func);
    config config_;
    asio::io_service service_;
    asio::posix::stream_descriptor stdin_;
    asio::posix::stream_descriptor stdout_;
    beam::message::buffer_pool pool_;
    std::queue<beam::message::capnproto::payload<robocup2Dsim::srprotocol::RefInput>> input_queue_;
    std::queue<beam::message::capnproto::payload<robocup2Dsim::srprotocol::RefOutput>> output_queue_;
};

} // namespace referee
} // namespace robocup2Dsim

#endif

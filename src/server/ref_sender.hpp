#ifndef ROBOBUP2DSIM_SERVER_REF_SENDER_HPP
#define ROBOBUP2DSIM_SERVER_REF_SENDER_HPP

#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/srprotocol/protocol.capnp.h>
#include <turbo/ipc/posix/pipe.hpp>

namespace robocup2Dsim {
namespace server {

class ref_sender
{
public:
    explicit ref_sender(turbo::ipc::posix::pipe::back&& ref_stdin);
    void send(beam::message::capnproto<robocup2Dsim::srprotocol::RefInput>& message);
private:
    ref_sender() = delete;
    ref_sender(const ref_sender&) = delete;
    ref_sender& operator=(const ref_sender&) = delete;
    turbo::ipc::posix::pipe::back ref_stdin_;
};

} // namespace server
} // namespace robocup2Dsim

#endif

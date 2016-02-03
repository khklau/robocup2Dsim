#include "ref_sender.hpp"
#include <beam/message/capnproto.hxx>
#include <capnp/serialize.h>

namespace robocup2Dsim {
namespace server {

ref_sender::ref_sender(turbo::ipc::posix::pipe::back&& ref_stdin) :
	ref_stdin_(std::move(ref_stdin))
{ }

void ref_sender::send(beam::message::capnproto<robocup2Dsim::srprotocol::RefInput>& message)
{
    capnp::writeMessageToFd(ref_stdin_.get_handle(), message.get_segments());
}

} // namespace server
} // namespace robocup2Dsim

#include "bot_sender.hpp"
#include <beam/message/capnproto.hxx>
#include <capnp/serialize.h>

namespace robocup2Dsim {
namespace client {

bot_sender::bot_sender(turbo::ipc::posix::pipe::back&& bot_stdin) :
	bot_stdin_(std::move(bot_stdin))
{ }

void bot_sender::send(beam::message::capnproto<robocup2Dsim::bcprotocol::ClientTransmission>& message)
{
    capnp::writeMessageToFd(bot_stdin_.get_handle(), message.get_segments());
}

} // namespace client
} // namespace robocup2Dsim

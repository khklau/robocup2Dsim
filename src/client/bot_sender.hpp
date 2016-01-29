#ifndef ROBOBUP2DSIM_CLIENT_BOT_SENDER_HPP
#define ROBOBUP2DSIM_CLIENT_BOT_SENDER_HPP

#include <beam/message/capnproto.hpp>
#include <robocup2Dsim/bcprotocol/protocol.capnp.h>
#include <turbo/ipc/posix/pipe.hpp>

namespace robocup2Dsim {
namespace client {

class bot_sender
{
public:
    explicit bot_sender(turbo::ipc::posix::pipe::back&& bot_stdin);
    void send(beam::message::capnproto<robocup2Dsim::bcprotocol::BotInput>& message);
private:
    bot_sender() = delete;
    bot_sender(const bot_sender&) = delete;
    bot_sender& operator=(const bot_sender&) = delete;
    turbo::ipc::posix::pipe::back bot_stdin_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

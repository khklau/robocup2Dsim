#ifndef ROBOCUP2DSIM_CLIENT_STATE_MACHINE_HPP
#define ROBOCUP2DSIM_CLIENT_STATE_MACHINE_HPP

#include <asio/io_service.hpp>
#include <beam/message/capnproto.hpp>
#include <turbo/container/spsc_ring_queue.hpp>
#include <turbo/ipc/posix/signal_notifier.hpp>
#include <turbo/process/posix/spawn.hpp>
#include "config.hpp"
#include "bot_receiver.hpp"
#include "bot_sender.hpp"
#include "specification.hpp"

namespace robocup2Dsim {
namespace client {

class state_machine
{
public:
    state_machine(const config& config, turbo::process::posix::child&& bot);
private:
    enum class state
    {
	nobot_unregistered,
	nobot_onbench,
	withbot_unregistered,
	withbot_onbench,
	withbot_playing
    };
    const config& config_;
    turbo::process::posix::child&& bot_;
    state state_;
    bot_in_queue_type bot_in_;
    bot_out_queue_type bot_out_;
    server_in_queue_type server_in_;
    server_out_queue_type server_out_;
    bot_receiver botrec_;
    bot_sender botsend_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

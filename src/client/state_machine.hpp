#ifndef ROBOCUP2DSIM_CLIENT_STATE_MACHINE_HPP
#define ROBOCUP2DSIM_CLIENT_STATE_MACHINE_HPP

#include <memory>
#include <asio/io_service.hpp>
#include <turbo/ipc/posix/signal_notifier.hpp>
#include <turbo/process/posix/spawn.hpp>
#include <robocup2Dsim/bcprotocol/protocol.hpp>
#include <robocup2Dsim/csprotocol/protocol.hpp>
#include "config.hpp"
#include "bot_io.hpp"
#include "server_io.hpp"
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
    std::unique_ptr<robocup2Dsim::bcprotocol::bot_input_queue_type> bot_input_queue_;
    std::unique_ptr<robocup2Dsim::bcprotocol::bot_output_queue_type> bot_output_queue_;
    std::unique_ptr<robocup2Dsim::csprotocol::client_status_queue_type> client_status_queue_;
    std::unique_ptr<robocup2Dsim::csprotocol::client_trans_queue_type> client_trans_queue_;
    std::unique_ptr<robocup2Dsim::csprotocol::server_status_queue_type> server_status_queue_;
    std::unique_ptr<robocup2Dsim::csprotocol::server_trans_queue_type> server_trans_queue_;
    bot_io bot_io_;
    server_io server_io_;
};

} // namespace client
} // namespace robocup2Dsim

#endif

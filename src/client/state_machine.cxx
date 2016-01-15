#include "state_machine.hpp"
#include <utility>
#include <turbo/container/spsc_ring_queue.hxx>

namespace robocup2Dsim {
namespace client {

state_machine::state_machine(const config& config, turbo::process::posix::child&& bot) :
	config_(config),
	bot_(std::move(bot)),
	state_(state::withbot_unregistered),
	bot_in_(config_.bot_msg_queue_length),
	bot_out_(config_.bot_msg_queue_length),
	server_in_(config_.server_msg_queue_length),
	server_out_(config_.server_msg_queue_length),
	botrec_(std::move(bot.out), bot_out_.get_producer()),
	botsend_(std::move(bot.in))
{ }

} // namespace client
} // namespace robocup2Dsim

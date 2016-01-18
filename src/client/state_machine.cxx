#include "state_machine.hpp"
#include <utility>
#include <turbo/container/spsc_ring_queue.hxx>

namespace robocup2Dsim {
namespace client {

state_machine::state_machine(const config& config, turbo::process::posix::child&& bot) :
	config_(config),
	bot_(std::move(bot)),
	state_(state::withbot_unregistered),
	bot_trans_queue_(config_.bot_msg_queue_length),
	client_status_queue_(config_.server_msg_queue_length),
	client_trans_queue_(config_.server_msg_queue_length),
	server_status_queue_(config_.server_msg_queue_length),
	server_trans_queue_(config_.server_msg_queue_length),
	botrec_(std::move(bot.out), bot_trans_queue_.get_producer()),
	botsend_(std::move(bot.in))
{ }

} // namespace client
} // namespace robocup2Dsim

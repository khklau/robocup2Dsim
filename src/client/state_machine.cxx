#include "state_machine.hpp"
#include <utility>
#include <turbo/container/spsc_ring_queue.hxx>

namespace robocup2Dsim {
namespace client {

state_machine::state_machine(const config& config, turbo::process::posix::child&& bot) :
	config_(config),
	bot_(std::move(bot)),
	state_(state::withbot_unregistered),
	bot_input_queue_(new robocup2Dsim::bcprotocol::bot_input_queue_type(config_.bot_msg_queue_length)),
	bot_output_queue_(new robocup2Dsim::bcprotocol::bot_output_queue_type(config_.bot_msg_queue_length)),
	client_status_queue_(new robocup2Dsim::csprotocol::client_status_queue_type(config_.server_msg_queue_length)),
	client_trans_queue_(new robocup2Dsim::csprotocol::client_trans_queue_type(config_.server_msg_queue_length)),
	server_status_queue_(new robocup2Dsim::csprotocol::server_status_queue_type(config_.server_msg_queue_length)),
	server_trans_queue_(new robocup2Dsim::csprotocol::server_trans_queue_type(config_.server_msg_queue_length)),
	botsend_(std::move(bot.in)),
	botrec_(std::move(bot.out), bot_output_queue_->get_producer())
{ }

} // namespace client
} // namespace robocup2Dsim

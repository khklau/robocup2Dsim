#include "referee.hpp"
#include <robocup2Dsim/runtime/db_access.hpp>

namespace bin = beam::internet;
namespace bmc = beam::message::capnproto;
namespace rcc = robocup2Dsim::common::command;
namespace rre = robocup2Dsim::referee;
namespace rse = robocup2Dsim::server;
namespace rsr = robocup2Dsim::srprotocol;
namespace rru = robocup2Dsim::runtime;

namespace robocup2Dsim {
namespace referee {

referee::referee(const config& conf)
    :
	config_(conf),
	enrollment_(new rse::enrollment()),
	roster_(),
	game_state_(),
	pool_(config_.msg_word_length, config_.msg_buffer_capacity)
{ }

void referee::process(std::queue<bmc::payload<rsr::RefInput>>& in, std::queue<bmc::payload<rsr::RefOutput>>& out)
{
    while (0U < in.size())
    {
	bmc::statement<rsr::RefInput> statement(std::move(in.front()));
	in.pop();

	rsr::RefInput::Reader input = statement.read();
	switch (input.which())
	{
	    case rsr::RefInput::RULES:
		break;
	    case rsr::RefInput::REGISTRATION:
		process(input.getRegistration(), out);
		break;
	}
    }
}

void referee::process(
	const rsr::RegistrationRequest::Reader& registration,
	std::queue<bmc::payload<rsr::RefOutput>>& out)
{
    rcc::Registration::Reader details = registration.getDetails();
    enrollment_->register_client(bin::endpoint_id(registration.getClient()), details);
    if (enrollment_->is_full())
    {
        bmc::form<rsr::RefOutput> ref_form(
                std::move(pool_.borrow()),
                bin::endpoint_id(registration.getClient()));
        rsr::RefOutput::Builder ref_output = ref_form.build();
        ref_output.setRosterFinalised();
        out.push(std::move(bmc::serialise(pool_, ref_form)));

        roster_ = enrollment_->finalise();
        enrollment_.reset();
        game_state_.reset(new rse::server_game_state(rru::update_local_db()));
    }
}

} // namespace referee
} // namespace robocup2Dsim

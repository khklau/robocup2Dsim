#include "test_utility.hpp"
#include <capnp/message.h>

namespace bin = beam::internet;
namespace rce = robocup2Dsim::common::entity;
namespace rcc = robocup2Dsim::common::command;
namespace rcm = robocup2Dsim::common::metadata;
namespace rse = robocup2Dsim::server;
namespace ttu = turbo::type_utility;

rse::enrollment::register_result register_client(bin::endpoint_id client, const std::string& team, rce::UniformNumber uniform, rce::PlayerType ptype, rse::enrollment& enrollment)
{
    capnp::MallocMessageBuilder arena;
    rcc::Registration::Builder request = arena.initRoot<rcc::Registration>();
    rcm::Version::Builder version = request.initVersion();
    version.setNumberA(1U);
    version.setNumberB(0U);
    version.setNumberC(0U);
    version.setNumberD(0U);
    request.setTeamName(team);
    request.setUniform(uniform);
    request.setPlayerType(ptype);
    return enrollment.register_client(client, request.asReader());
}

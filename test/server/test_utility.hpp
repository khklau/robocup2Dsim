#include <robocup2Dsim/server/roster.hpp>
#include <robocup2Dsim/common/entity.capnp.h>
#include <beam/internet/endpoint.hpp>
#include <string>

robocup2Dsim::server::enrollment::register_result register_client(
        beam::internet::endpoint_id client,
        const std::string& team,
        robocup2Dsim::common::entity::UniformNumber uniform,
        robocup2Dsim::common::entity::PlayerType ptype,
        robocup2Dsim::server::enrollment& enrollment);

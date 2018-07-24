@0xac0fbc0729c65236;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::srprotocol");

using Metadata = import "/common/metadata.capnp";
using CommonCommand = import "/common/command.capnp";

struct RegistrationRequest
{
    endPoint @0 :Metadata.EndPointId;
    details @1 :CommonCommand.Registration;
}

struct RosterFinalised
{
}

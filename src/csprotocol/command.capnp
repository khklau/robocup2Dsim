@0xc7bd906a94321b78;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Entity = import "/common/entity.capnp";
using Field = import "/common/field.capnp";
using Metadata = import "/common/metadata.capnp";
using Physics = import "/common/physics.capnp";
using Rule = import "/common/rule.capnp";
using CommonCommand = import "/common/command.capnp";

struct VersionMismatchError { }
struct TeamSlotTakenError { }
struct TeamFullError { }
struct UniformTakenError { }
struct GoalieTakenError { }

struct RegistrationRequest
{
    details @0 :CommonCommand.Registration;
}

struct RegistrationError
{
    union
    {
	versionMismatch @0 :VersionMismatchError;
	teamTaken @1 :TeamSlotTakenError;
	teamFull @2 :TeamFullError;
	uniformTaken @3 :UniformTakenError;
	goalieTaken @4 :GoalieTakenError;
    }
}

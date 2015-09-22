@0xc7bd906a94321b78;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Entity = import "/core/entity.capnp";
using Field = import "/core/field.capnp";
using Metadata = import "/core/metadata.capnp";
using Physics = import "/core/physics.capnp";
using Rule = import "/core/rule.capnp";
using CoreCommand = import "/core/command.capnp";

struct VersionMismatchError { }
struct TeamSlotTakenError { }
struct TeamFullError { }
struct UniformTakenError { }
struct GoalieTakenError { }

struct InitRequest
{
    registration @0 :CoreCommand.Registration;
}

struct InitReply
{
    union
    {
	open @0 :CoreCommand.MatchOpen;
	error @1 :InitError;
    }
}

struct InitError
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

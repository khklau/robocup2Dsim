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
struct NoSuchTeamError { }
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
	result @0 :InitResult;
	error @1 :InitError;
    }
}

struct InitResult
{
    playMode @0 :Rule.PlayMode;
}

struct InitError
{
    union
    {
	versionMismatch @0 :VersionMismatchError;
	noSuchTeam @1 :NoSuchTeamError;
	teamFull @2 :TeamFullError;
	uniformTaken @3 :UniformTakenError;
	goalieTaken @4 :GoalieTakenError;
    }
}

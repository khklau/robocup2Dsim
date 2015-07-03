@0xc7bd906a94321b78;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Entity = import "/core/entity.capnp";
using Metadata = import "/core/metadata.capnp";
using Rule = import "/core/rule.capnp";

struct VersionMismatchError { }
struct NoSuchTeamError { }
struct TeamFullError { }
struct UniformTakenError { }
struct GoalieTakenError { }
struct GameInPlayOnError { }

struct InitRequest
{
    version @0 :Metadata.Version;
    teamName @1 :Text;
    uniform @2 :Entity.UniformNumber;
    playerType @3 :Entity.PlayerType;
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
    side @0 :Entity.Side;
    playMode @1 :Rule.PlayMode;
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

struct ReconnectRequest
{
    teamName @0 :Text;
    uniform @1 :Entity.UniformNumber;
}

struct ReconnectReply
{
    union
    {
	result @0 :ReconnectResult;
	error @1 :ReconnectError;
    }
}

struct ReconnectResult
{
    side @0 :Entity.Side;
    playMode @1 :Rule.PlayMode;
}

struct ReconnectError
{
    union
    {
	noSuchTeam @0 :NoSuchTeamError;
	teamFull @1 :TeamFullError;
	uniformTaken @2 :UniformTakenError;
	gameInPlayOn @3 :GameInPlayOnError;
    }
}

struct ByeRequest
{
    teamName @0 :Text;
    uniform @1 :Entity.UniformNumber;
}

struct ByeReply
{
    teamName @0 :Text;
    uniform @1 :Entity.UniformNumber;
}

@0xc7bd906a94321b78;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Entity = import "/src/core/entity.capnp";
using Metadata = import "/src/core/metadata.capnp";
using Rule = import "/src/core/rule.capnp";

struct ErrorVersionMismatch { }
struct ErrorNoSuchTeam { }
struct ErrorTeamFull { }
struct ErrorUniformTaken { }
struct ErrorGoalieTaken { }
struct ErrorGameInPlayOn { }

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
	versionMismatch @0 :ErrorVersionMismatch;
	noSuchTeam @1 :ErrorNoSuchTeam;
	teamFull @2 :ErrorTeamFull;
	uniformTaken @3 :ErrorUniformTaken;
	goalieTaken @4 :ErrorGoalieTaken;
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
	noSuchTeam @0 :ErrorNoSuchTeam;
	teamFull @1 :ErrorTeamFull;
	uniformTaken @2 :ErrorUniformTaken;
	gameInPlayOn @3 :ErrorGameInPlayOn;
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

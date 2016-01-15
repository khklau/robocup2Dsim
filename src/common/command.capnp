@0xc6f85ffb87e200e2;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

using Entity = import "entity.capnp";
using Metadata = import "metadata.capnp";
using Rule = import "rule.capnp";

struct Registration
{
    version @0 :Metadata.Version;
    teamName @1 :Entity.TeamName;
    uniform @2 :Entity.UniformNumber;
    playerType @3 :Entity.PlayerType;
}

struct MatchOpen
{
    playMode @0 :Rule.PlayMode;
}

struct MatchClose
{
    finalScore @0 :Rule.Score;
}

enum AbortReason
{
    registrationTimeOut @0;
    otherTeamDisconnect @1;
}

struct MatchAbort
{
    reason @0 :AbortReason;
    score @1 :Rule.Score;
}

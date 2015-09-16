@0xc6f85ffb87e200e2;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Entity = import "entity.capnp";
using Metadata = import "metadata.capnp";
using Rule = import "rule.capnp";

struct Registration
{
    version @0 :Metadata.Version;
    teamName @1 :Text;
    uniform @2 :Entity.UniformNumber;
    playerType @3 :Entity.PlayerType;
}

struct MatchOver
{
    finalScore @0 :Rule.Score;
}

enum AbortReason
{
    registrationTimeOut @0;
    otherTeamDisconnect @1;
}

struct MatchAborted
{
    reason @0 :AbortReason;
    score @1 :Rule.Score;
}

@0x80022fa8c81bf3d3;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

using Entity = import "entity.capnp";
using Field = import "field.capnp";
using Metadata = import "metadata.capnp";
using Physics = import "physics.capnp";
using Rule = import "rule.capnp";

struct KickOffPlan
{
    schedule @0 :Physics.NanoSecond;
    session @1 :Rule.PlaySession;
    kickingSide @2 :Field.Side;
}

struct FieldOpen
{
    time @0 :Physics.NanoSecond;
    side @1 :Field.Side;
    position @2 :Physics.Position;
    plan @3 :KickOffPlan;
}

struct KickOff
{
    time @0 :Physics.NanoSecond;
}

struct TimeOver
{
    time @0 :Physics.NanoSecond;
    currentScore @1 :Rule.Score;
    plan @2 :KickOffPlan;
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

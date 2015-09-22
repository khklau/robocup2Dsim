@0xc9dafaeac85c954c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Entity = import "/core/entity.capnp";
using Physics = import "/core/physics.capnp";
using Rule = import "/core/rule.capnp";

struct ScoreQuery { }

struct ScoreResult
{
    time @0 :Physics.NanoSecond;
    ourScore @1 :Rule.Score;
    theirScore @2 :Rule.Score;
}

struct TimeQuery { }

struct TimeResult
{
    time @0 :Physics.NanoSecond;
}

struct RegistrationQuery { }

struct TeamRegistrationStatus
{
    teamName @0 :Entity.TeamName;
    playerCount @1 :UInt8;
}

struct RegistrationResult
{
    startTime @0 :Physics.NanoSecond;
    deadLine @1 :Physics.NanoSecond;
    leftTeam @2 :TeamRegistrationStatus;
    rightTeam @3 :TeamRegistrationStatus;
}

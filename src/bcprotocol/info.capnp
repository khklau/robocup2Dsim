@0xc9dafaeac85c954c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Physics = import "/core/physics.capnp";
using Rule = import "/core/rule.capnp";

struct ScoreQuery { }

struct ScoreResult
{
    time @0 :Physics.Nanoseconds;
    ourScore @1 :Rule.Score;
    theirScore @2 :Rule.Score;
}

struct TimeQuery { }

struct TimeResult
{
    time @0 :Physics.Nanoseconds;
}

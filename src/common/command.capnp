@0xc6f85ffb87e200e2;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common::command");

using Entity = import "entity.capnp";
using Metadata = import "metadata.capnp";
using Physics = import "physics.capnp";
using Rule = import "rule.capnp";

struct Registration
{
    version @0 :Metadata.Version;
    teamName @1 :Entity.TeamName;
    uniform @2 :Entity.UniformNumber;
    playerType @3 :Entity.PlayerType;
}

struct MatchStatus
{
    playMode @0 :Rule.PlayMode;
    score @1 :Rule.Score;
    gameTime @2 :Physics.NanoSecond;
}

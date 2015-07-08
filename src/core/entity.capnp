@0xc98f2d65bc9a57fc;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Field = import "field.capnp";
using Physics = import "/core/physics.capnp";

enum PlayerType
{
    outfield @0;
    goalie @1;
}

enum UniformNumber
{
    one @0;
    two @1;
    three @2;
    four @3;
    five @4;
    six @5;
    seven @6;
    eight @7;
    nine @8;
    ten @9;
    eleven @10;
}

struct PlayerId
{
    side @0 :Field.Side;
    uniform @1 :UniformNumber;
    type @2 :PlayerType;
}

struct Player
{
    id @0 :PlayerId;
}

enum ParticipantType
{
    player @0;
    coach @1;
    referee @2;
    trainer @3;
}

struct Ball {}

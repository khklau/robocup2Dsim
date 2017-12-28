@0xc98f2d65bc9a57fc;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common::entity");

using Field = import "field.capnp";
using Physics = import "/common/physics.capnp";

using TeamName = Text;

enum PlayerType
{
    outfield @0;
    goalKeeper @1;
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

enum TeamId
{
    alpha @0;
    beta @1;
}

struct PlayerUniform
{
    uniform @0 :UniformNumber;
    team @1 :TeamId;
}

struct Player
{
    id @0 :PlayerUniform;
    type @1 :PlayerType;
    position @2 :Physics.Position;
    stamina @3 :Physics.KiloJoule;
    effort @4 :Physics.KiloJoule;
    bodyDirection @5 :Physics.Degree;
    headDirection @6 :Physics.Degree;
    velocity @7 :Physics.MetresPerHour;
}

enum ParticipantType
{
    player @0;
    coach @1;
    referee @2;
    trainer @3;
}

struct Ball
{
    position @0 :Physics.Position;
    direction @1 :Physics.Degree;
    velocity @2 :Physics.MetresPerHour;
}

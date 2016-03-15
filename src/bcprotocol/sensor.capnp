@0xe18889eda84ec4fa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Entity = import "/common/entity.capnp";
using Field = import "/common/field.capnp";
using Physics = import "/common/physics.capnp";
using Rule = import "/common/rule.capnp";

struct Announcement
{
    time @0 :Physics.NanoSecond;
    direction@1 :Physics.Degree;
    message @2 :AnyPointer;
}

struct Instruction
{
    time @0 :Physics.NanoSecond;
    message @1 :AnyPointer;
}

struct Communication
{
    union
    {
	announcement @0 :Announcement;
	instruction @1 :Instruction;
    }
}

struct PlayerVisual1
{
    id @0 :Entity.PlayerId;
    direction @1 :Physics.Degree;
    distance @2 :Physics.MilliMetre;
    body @3 :Physics.Degree;
    head @4 :Physics.Degree;
}

struct PlayerVisual2
{
    side @0 :Field.Side;
    direction @1 :Physics.Degree;
    distance @2 :Physics.MilliMetre;
}

struct PlayerVisual3
{
    direction @0 :Physics.Degree;
}

struct BallVisual1
{
    direction @0 :Physics.Degree;
    distance @1 :Physics.MilliMetre;
}

struct BallVisual2
{
    direction @0 :Physics.Degree;
}

struct PointVisual1
{
    id @0 :Field.PointId;
    direction @1 :Physics.Degree;
    distance @2 :Physics.MilliMetre;
}

struct PointVisual2
{
    id @0 :Field.PointId;
    direction @1 :Physics.Degree;
}

struct LineVisual1
{
    id @0 :Field.LineId;
    direction @1 :Physics.Degree;
    incidenceAngle @2 :Physics.Degree;
    distance @3 :Physics.MilliMetre;
}

struct LineVisual2
{
    id @0 :Field.LineId;
    direction @1 :Physics.Degree;
    incidenceAngle @2 :Physics.Degree;
}

struct Visual
{
    time @0 :Physics.NanoSecond;
    objectId :union
    {
	player1 @1 :PlayerVisual1;
	player2 @2 :PlayerVisual2;
	player3 @3 :PlayerVisual3;
	ball1 @4 :BallVisual1;
	ball2 @5 :BallVisual2;
	point1 @6 :PointVisual1;
	point2 @7 :PointVisual2;
	line1 @8 :LineVisual1;
	line2 @9 :LineVisual2;
    }
}

@0xe18889eda84ec4fa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Entity = import "/core/entity.capnp";
using Field = import "/core/field.capnp";
using Physics = import "/core/physics.capnp";
using Rule = import "/core/rule.capnp";

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

struct KickOffPlan
{
    schedule @0 :Physics.NanoSecond;
    session @1 :Rule.PlaySession;
    kickingSide @2 :Field.Side;
}

struct FieldOpen
{
    side @0 :Field.Side;
    position @1 :Physics.Position;
    plan @2 :KickOffPlan;
}

struct TimeOver
{
    currentScore @0 :Rule.Score;
    plan @1 :KickOffPlan;
}

struct GameOver
{
    finalScore @0 :Rule.Score;
}

struct Judgment
{
    time @0 :Physics.NanoSecond;
    union
    {
	regTimeOut @1 :Void;
	open @2 :FieldOpen;
	kickOff @3 :Void;
	timeOver @4 :TimeOver;
	gameOver @5 :GameOver;
    }
}

struct Communication
{
    union
    {
	announcement @0 :Announcement;
	instruction @1 :Instruction;
	judgment @2 :Judgment;
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

@0xe18889eda84ec4fa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Entity = import "/core/entity.capnp";
using Field = import "/core/field.capnp";
using Physics = import "/core/physics.capnp";

struct Announcement
{
    time @0 :Physics.Nanoseconds;
    direction@1 :Physics.Degree;
    message @2 :AnyPointer;
}

struct Instruction
{
    time @0 :Physics.Nanoseconds;
    message @1 :AnyPointer;
}

struct Judgment
{
    time @0 :Physics.Nanoseconds;
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

struct PlayerObservation1
{
    id @0 :Entity.PlayerId;
    distance @1 :Physics.Millimeter;
    direction @2 :Physics.Degree;
}

struct PlayerObservation2
{
    id @0 :Entity.PlayerId;
    distance @1 :Physics.Millimeter;
    direction @2 :Physics.Degree;
}

struct PlayerObservation3
{
    id @0 :Entity.PlayerId;
    distance @1 :Physics.Millimeter;
    direction @2 :Physics.Degree;
}

struct PlayerObservation4
{
    id @0 :Entity.PlayerId;
    distance @1 :Physics.Millimeter;
    direction @2 :Physics.Degree;
}

struct Observation
{
    time @0 :Physics.Nanoseconds;
    objectId :union
    {
	player @1 :Entity.PlayerId;
	ball @2 :Void;
	boundaryPoint @3 :Field.BoundaryPointId;
	boundaryLine @4 :Field.BoundaryLineId;
	goal @5 :Field.GoalPointId;
	goalBox @6 :Field.GoalBoxPointId;
	penaltyBox @7 :Field.PenaltyBoxPointId;
	penaltyArc @8 :Field.PenaltyArcPointId;
	centreCircle @9 :Field.CentreCirclePointId;
    }
}

@0xb65c1fbcbbf67993;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Physics = import "/core/physics.capnp";

enum Side
{
    left @0;
    right @1;
}

enum Aspect
{
    top @0;
    bottom @1;
}

enum CornerFlagId
{
    topLeft @0;
    topRight @1;
    bottomLeft @2;
    bottomRight @3;
}

struct CornerFlag
{
    id @0 :CornerFlagId;
    position @1 :Physics.Position;
}

enum BoundaryLineId
{
    left @0;
    right @1;
    top @2;
    bottom @3;
}

struct Boundary
{
    topLeft @0 :CornerFlag;
    topRight @1 :CornerFlag;
    bottomLeft @2 :CornerFlag;
    bottomRight @3 :CornerFlag;
}

enum CentreLinePointId
{
    top @0;
    bottom @1;
}

struct CentreLinePoint
{
    id @0 :CentreLinePointId;
    position @1 :Physics.Position;
}

struct CentreCircle
{
    centre @0 :Physics.Position;
    radius @1 :Physics.MilliMetre;
}

struct GoalPostId
{
    side @0 :Side;
    aspect @1 :Aspect;
}

struct GoalPost
{
    id @0 :GoalPostId;
    position @1 :Physics.Position;
}

enum GoalLineId
{
    left @0;
    right @1;
}

struct Goal
{
    id @0 :GoalLineId;
    top @1 :GoalPost;
    bottom @2 :GoalPost;
}

struct BoxCorner
{
    side @0 :Side;
    aspect @1 :Aspect;
}

struct GoalBoxCornerId
{
    side @0 :Side;
    corner @1 :BoxCorner;
}

struct GoalBoxCorner
{
    id @0 :GoalBoxCornerId;
    position @1 :Physics.Position;
}

enum GoalBoxLineId
{
    left @0;
    right @1;
    top @2;
    bottom @3;
}

struct GoalBox
{
    topLeft @0 :GoalBoxCorner;
    topRight @1 :GoalBoxCorner;
    bottomLeft @2 :GoalBoxCorner;
    bottomRight @3 :GoalBoxCorner;
}

struct PenaltyBoxCornerId
{
    side @0 :Side;
    corner @1 :BoxCorner;
}

struct PenaltyBoxCorner
{
    id @0 :PenaltyBoxCornerId;
    position @1 :Physics.Position;
}

enum PenaltyBoxLineId
{
    left @0;
    right @1;
    top @2;
    bottom @3;
}

struct PenaltyBox
{
    topLeft @0 :PenaltyBoxCorner;
    topRight @1 :PenaltyBoxCorner;
    bottomLeft @2 :PenaltyBoxCorner;
    bottomRight @3 :PenaltyBoxCorner;
}

struct PenaltyArcId
{
    side @0 :Side;
}

struct PenaltyArc
{
    id @0 :PenaltyArcId;
    centre @1 :Physics.Position;
    radius @2 :Physics.MilliMetre;
}

enum Latitude
{
    above30 @0;
    above20 @1;
    above10 @2;
    center0 @3;
    below10 @4;
    below20 @5;
    below30 @6;
}

struct ShortWallPointId
{
    side @0 :Side;
    point @1 :Latitude;
}

enum Longitude
{
    left50 @0;
    left40 @1;
    left30 @2;
    left20 @3;
    left10 @4;
    center0 @5;
    right10 @6;
    right20 @7;
    right30 @8;
    right40 @9;
    right50 @10;
}

struct LongWallPointId
{
    aspect @0 :Aspect;
    point @1 :Longitude;
}

struct LineId
{
    union
    {
	boundary @0 :BoundaryLineId;
	centreLine @1 :Void;
	centreCircle @2 :Void;
	goalLine @3 :GoalLineId;
	goalBox @4 :GoalBoxLineId;
	penaltyBox @5 :PenaltyBoxLineId;
	penaltyArc @6 :PenaltyArcId;
    }
}

struct PointId
{
    union
    {
	cornerFlag @0 :CornerFlagId;
	centreSpot @1 :Void;
	goalPost @2 :GoalPostId;
	penaltySpot @3 :PenaltyArcId;
	shortWallPoint @4 :ShortWallPointId;
	longWallPoint @5 :LongWallPointId;
    }
}

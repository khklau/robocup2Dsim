@0xb65c1fbcbbf67993;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Physics = import "/core/physics.capnp";

enum BoundaryPointId
{
    leftTop @0;
    leftBottom @1;
    centreTop @2;
    centreBottom @3;
    rightTop @4;
    rightBottom @5;
}

enum BoundaryLineId
{
    left @0;
    right @1;
    top @2;
    bottom @3;
}

enum Side
{
    left @0;
    right @1;
}

enum Aspect
{
    top @0;
    centre @1;
    bottom @2;
}

struct GoalPointId
{
    side @0 :Side;
    aspect @1 :Aspect;
}

struct GoalBoxPointId
{
    side @0 :Side;
    aspect @1 :Aspect;
}

struct PenaltyBoxPointId
{
    side @0 :Side;
    aspect @1 :Aspect;
}

struct PenaltyArcPointId
{
    side @0 :Side;
    aspect @1 :Aspect;
}

struct CentreCirclePointId
{
    side @0 :Side;
    aspect @1 :Aspect;
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
    enum Aspect
    {
	top @0;
	bottom @1;
    }
    aspect @0 :LongWallPointId.Aspect;
    point @1 :Longitude;
}

struct Goal
{
    side @0 :Side;
}

struct Line
{
    point1 @0 :Physics.Position;
    point2 @1 :Physics.Position;
}

struct Flag {}

struct Spot { }

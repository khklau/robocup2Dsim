@0x90cf51a28ca78630;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

using Day = UInt32;
using Degree = Int16;
using KiloJoule = UInt8;
using MetresPerHour = Int16;
using Minutes = UInt16;
using DegreesPerSecond = Int16;
using MilliMetre = Float32;
using NanoSecond = UInt64;
using Newton = Int8;

const minBodyDegree :Degree = -180;
const maxBodyDegree :Degree = 180;
const minHeadDegree :Degree = -90;
const maxHeadDegree :Degree = 90;
const minNewton :Newton = -100;
const maxNewton :Newton = 100;

struct Position
{
    x @0 :Float32;
    y @1 :Float32;
}

struct Line
{
    point1 @0 :Position;
    point2 @1 :Position;
}

struct Rectangle
{
    topLeft @0 :Position;
    topRight @1 :Position;
    bottomLeft @2 :Position;
    bottomRight @3 :Position;
}

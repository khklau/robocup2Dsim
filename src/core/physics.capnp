@0x90cf51a28ca78630;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Day = UInt32;
using Degree = Int16;
using Joules = Uint16;
using MetersPerSecond = Float32;
using Millimeter = Float32;
using Nanoseconds = UInt64;
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

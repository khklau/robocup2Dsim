@0x90cf51a28ca78630;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Degree = Int16;
using Newton = Int8;
using Time = UInt64;
using Date = UInt32;

const minBodyDegree :Degree = -180;
const maxBodyDegree :Degree = 180;
const minHeadDegree :Degree = -90;
const maxHeadDegree :Degree = 90;
const minNewton :Newton = -100;
const maxNewton :Newton = 100;

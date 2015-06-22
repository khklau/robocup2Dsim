@0x90cf51a28ca78630;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Degree = Int16;
using Newton = Int8;

const minDegree :Degree = -180;
const maxDegree :Degree = 180;
const minNewton :Newton = -100;
const maxNewton :Newton = 100;

@0xc61bac6a340f9df1;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using SequenceNumber = UInt16;
using FrameNumber = UInt32;

struct Version
{
    numberA @0 :UInt8;
    numberB @1 :UInt8;
    numberC @2 :UInt8;
    numberD @3 :UInt8;
}

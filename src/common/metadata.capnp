@0xc61bac6a340f9df1;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common::metadata");

using ChannelId = UInt8;
using FrameNumber = UInt32;
using SequenceNumber = UInt16;
using EndPointId = UInt64;   # needs to be kept in sync with beam::internet::endpoint_id

struct Version
{
    numberA @0 :UInt8;
    numberB @1 :UInt8;
    numberC @2 :UInt8;
    numberD @3 :UInt8;
}

const currentVersion :Version = ( numberA = 1, numberB = 0, numberC = 0, numberD = 0 );

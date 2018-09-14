@0xb508fc49a822173b;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Metadata = import "/common/metadata.capnp";
using Physics = import "/common/physics.capnp";

struct Ping
{
    sequence @0 :Metadata.SequenceNumber;
}

struct Pong
{
    sequence @0 :Metadata.SequenceNumber;
    now @1 :Physics.NanoSecond;
}

struct PlayerStatus
{
    frame @0 :Metadata.FrameNumber;
    status @1 :AnyPointer;
}

struct Snapshot
{
    frame @0 :Metadata.FrameNumber;
    union
    {
        diff @1 :AnyPointer;
        full @2 :AnyPointer;
    }
}

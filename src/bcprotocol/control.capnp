@0xefbea916e22f4681;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Physics = import "/src/core/physics.capnp";

struct CatchInstruction
{
    direction @0 :Physics.Degree;
}

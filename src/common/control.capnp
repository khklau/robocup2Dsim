@0xb4dcc02a0871b334;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

using Physics = import "physics.capnp";

struct MoveFootAction
{
    velocity @0 :Physics.MetresPerHour;
}

struct CatchAction
{
    velocity @0 :Physics.MetresPerHour;
}

struct Control
{
    union
    {
	moveFoot @0 :MoveFootAction;
	catch @1 :CatchAction;
    }
}

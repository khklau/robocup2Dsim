@0xefbea916e22f4681;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using CommonControl = import "/common/control.capnp";
using Physics = import "/common/physics.capnp";

struct RunAction
{
    velocity @0 :Physics.MetresPerHour;
}

struct TurnHeadAction
{
    velocity @0 :Physics.DegreesPerSecond;
}

struct TurnTorsoAction
{
    velocity @0 :Physics.DegreesPerSecond;
}

struct Control
{
    union
    {
	moveFoot @0 :CommonControl.MoveFootAction;
	run @1 :RunAction;
	turnHead @2 :TurnHeadAction;
	turnTorso @3 :TurnTorsoAction;
    }
}

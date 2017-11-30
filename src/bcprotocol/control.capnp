@0xefbea916e22f4681;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using CommonControl = import "/common/control.capnp";
using Physics = import "/common/physics.capnp";

struct Control
{
    union
    {
	moveFoot @0 :CommonControl.MoveFootAction;
	catch @1 :CommonControl.CatchAction;
	run @2 :CommonControl.RunAction;
	turnHead @3 :CommonControl.TurnHeadAction;
	turnTorso @4 :CommonControl.TurnTorsoAction;
    }
}

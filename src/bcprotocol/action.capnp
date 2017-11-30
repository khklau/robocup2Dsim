@0xefbea916e22f4681;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using CommonAction = import "/common/action.capnp";
using Physics = import "/common/physics.capnp";

struct PlayerAction
{
    union
    {
	moveFoot @0 :CommonAction.MoveFootAction;
	catch @1 :CommonAction.CatchAction;
	run @2 :CommonAction.RunAction;
	turnHead @3 :CommonAction.TurnHeadAction;
	turnTorso @4 :CommonAction.TurnTorsoAction;
    }
}

@0xefbea916e22f4681;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using CommonControl = import "/common/control.capnp";
using Physics = import "/common/physics.capnp";

struct DashAction
{
    power @0 :Physics.Newton;
}

struct TurnBodyAction
{
    direction @0 :Physics.Degree;
}

struct TurnHeadAction
{
    direction @0 :Physics.Degree;
}

struct Control
{
    union
    {
	catch @0 :CommonControl.CatchAction;
	dash @1 :DashAction;
	kick @2 :CommonControl.KickAction;
	turnBody @3 :TurnBodyAction;
	turnHead @4 :TurnHeadAction;
    }
}

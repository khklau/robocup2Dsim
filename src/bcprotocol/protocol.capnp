@0xca7b62e1bed2bc72;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Control = import "control.capnp";
using Error = import "/src/core/error.capnp";
using Info = import "info.capnp";

struct BotTransmission
{
    union
    {
	catch @0 :Control.CatchAction;
	dash @1 :Control.DashAction;
	kick @2 :Control.KickAction;
	turnBody @3 :Control.TurnBodyAction;
	turnHead @4 :Control.TurnHeadAction;
	scoreQuery @5 :Info.ScoreQuery;
	timeQuery @6 :Info.TimeQuery;
    }
}

struct ClientTransmission
{
    union
    {
	unknownMsg @0 :Error.UnknownMsgError;
	malformedMsg @1 :Error.MalformedMsgError;
	scoreResult @2 :Info.ScoreResult;
	timeResult @3 :Info.TimeResult;
    }
}

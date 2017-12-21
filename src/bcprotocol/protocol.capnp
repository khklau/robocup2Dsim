@0xca7b62e1bed2bc72;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Command = import "/common/command.capnp";
using CommonJudgement = import "/common/judgement.capnp";
using CommonAction = import "/common/action.capnp";
using Error = import "/common/error.capnp";
using Info = import "info.capnp";
using Sensor = import "sensor.capnp";

struct BotOutput
{
    union
    {
	action @0 :CommonAction.PlayerAction;
	query @1 :Info.QueryRequest;
	shutDown @2 :Void;
	crash @3 :Void;
    }
}

struct BotInput
{
    union
    {
	unknownMsg @0 :Error.UnknownMsgError;
	malformedMsg @1 :Error.MalformedMsgError;
	scoreResult @2 :Info.ScoreResult;
	timeResult @3 :Info.TimeResult;
	communication @4 :Sensor.Communication;
	visual @5 :Sensor.Visual;
	registration @6 :Command.Registration;
	status @7 :Command.MatchStatus;
	fieldOpen @8 :CommonJudgement.FieldOpen;
	kickOff @9 :CommonJudgement.KickOff;
	timeOver @10 :CommonJudgement.TimeOver;
	matchClose @11 :CommonJudgement.MatchClose;
	matchAbort @12 :CommonJudgement.MatchAbort;
	terminate @13 :Void;
    }
}

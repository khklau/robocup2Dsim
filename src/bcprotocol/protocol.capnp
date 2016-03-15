@0xca7b62e1bed2bc72;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Command = import "/common/command.capnp";
using CommonJudgement = import "/common/judgement.capnp";
using Control = import "control.capnp";
using Error = import "/common/error.capnp";
using Info = import "info.capnp";
using Sensor = import "sensor.capnp";

struct BotOutput
{
    union
    {
	control @0 :Control.Control;
	query @1 :Info.QueryRequest;
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
    }
}

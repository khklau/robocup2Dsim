@0xcc1ebf0ec96c3d15;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::srprotocol");

using CommonCommand = import "/common/command.capnp";
using Error = import "/common/error.capnp";
using Judgement = import "/common/judgement.capnp";
using Rule = import "/common/rule.capnp";
using Command = import "command.capnp";

struct RefOutput
{
    union
    {
	refReady @0 :Void;
	rosterFinalised @1 :Void;
	fieldOpen @2 :Judgement.FieldOpen;
	matchClose @3 :Judgement.MatchClose;
	matchAbort @4 :Judgement.MatchAbort;
	playJudgement @5 :Judgement.PlayJudgement;
	refCrashed @6 :Void;
    }
}

struct RefInput
{
    union
    {
	rules @0 :Rule.MatchRules;
	registration @1 :Command.RegistrationRequest;
    }
}

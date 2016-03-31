@0xcc1ebf0ec96c3d15;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::srprotocol");

using Command = import "/common/command.capnp";
using Error = import "/common/error.capnp";
using Judgement = import "/common/judgement.capnp";

struct RefOutput
{
    union
    {
	refReady @0 :Void;
	fieldOpen @1 :Judgement.FieldOpen;
	matchClose @2 :Judgement.MatchClose;
	matchAbort @3 :Judgement.MatchAbort;
	playJudgement @4 :Judgement.PlayJudgement;
	refCrashed @5 :Void;
    }
}

struct RefInput
{
}

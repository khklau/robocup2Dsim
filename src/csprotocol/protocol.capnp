@0xabe13ea5f0adf862;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Command = import "command.capnp";
using CommonCommand = import "/common/command.capnp";
using CommonAction = import "/common/action.capnp";
using CommonJudgement = import "/common/judgement.capnp";
using CommonMetadata = import "/common/metadata.capnp";
using Error = import "/common/error.capnp";
using Metadata = import "/common/metadata.capnp";

struct ClientStatus
{
    frame @0 :CommonMetadata.FrameNumber;
    status @1 :AnyPointer;
}

struct ClientTransaction
{
    union
    {
	registration @0 :Command.RegistrationRequest;
	action @1 :CommonAction.PlayerAction;
	disconnect @2 :Void;
    }
}

struct ServerStatus
{
    frame @0 :CommonMetadata.FrameNumber;
    snapshot @1 :AnyPointer;
}

struct ServerTransaction
{
    union
    {
	unknownMsg @0 :Error.UnknownMsgError;
	malformedMsg @1 :Error.MalformedMsgError;
	disconnect @2 :Void;
	regSuccess @3 :Void;
	regError @4 :Command.RegistrationError;
	fieldOpen @5 :CommonJudgement.FieldOpen;
	kickOff @6 :CommonJudgement.KickOff;
	timeOver @7 :CommonJudgement.TimeOver;
	matchClose @8 :CommonJudgement.MatchClose;
	matchAbort @9 :CommonJudgement.MatchAbort;
	playJudgement @10 :CommonJudgement.PlayJudgement;
    }
}

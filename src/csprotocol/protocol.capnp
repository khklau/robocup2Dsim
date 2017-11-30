@0xabe13ea5f0adf862;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Command = import "command.capnp";
using CommonCommand = import "/common/command.capnp";
using CommonAction = import "/common/action.capnp";
using CommonJudgement = import "/common/judgement.capnp";
using Error = import "/common/error.capnp";
using Metadata = import "/common/metadata.capnp";

using RecipientId = UInt64;

struct ClientStatus
{
    status @0 :AnyPointer;
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
    recipient @0 :RecipientId;
    snapshot @1 :AnyPointer;
}

struct ServerTransaction
{
    recipient @0 :RecipientId;
    union
    {
	unknownMsg @1 :Error.UnknownMsgError;
	malformedMsg @2 :Error.MalformedMsgError;
	disconnect @3 :Void;
	regSuccess @4 :Void;
	regError @5 :Command.RegistrationError;
	fieldOpen @6 :CommonJudgement.FieldOpen;
	kickOff @7 :CommonJudgement.KickOff;
	timeOver @8 :CommonJudgement.TimeOver;
	matchClose @9 :CommonJudgement.MatchClose;
	matchAbort @10 :CommonJudgement.MatchAbort;
	playJudgement @11 :CommonJudgement.PlayJudgement;
    }
}

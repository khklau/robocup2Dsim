@0xabe13ea5f0adf862;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Command = import "command.capnp";
using Status = import "status.capnp";
using CommonCommand = import "/common/command.capnp";
using Action = import "/common/action.capnp";
using Judgement = import "/common/judgement.capnp";
using Error = import "/common/error.capnp";
using Metadata = import "/common/metadata.capnp";

struct ClientStatus
{
    union
    {
        player @0 :Void;
        pong @1 :Void;
        playerAndPong @2 :Void;
    }
    value1 @3 :AnyPointer;
    value2 @4 :AnyPointer;
}

struct ClientTransaction
{
    union
    {
	registration @0 :Command.RegistrationRequest;
	action @1 :Action.PlayerAction;
	disconnect @2 :Void;
    }
}

struct ServerStatus
{
    union
    {
        snapshot @0 :Void;
        ping @1 :Void;
        snapsotAndPing @2 :Void;
    }
    value1 @3 :AnyPointer;
    value2 @4 :AnyPointer;
}

struct ServerTransaction
{
    union
    {
	unknownMsg @0 :Error.UnknownMsgError;
	malformedMsg @1 :Error.MalformedMsgError;
	disconnect @2 :Void;
	regAck @3 :Command.RegistrationAck;
	regError @4 :Command.RegistrationError;
	fieldOpen @5 :Judgement.FieldOpen;
	kickOff @6 :Judgement.KickOff;
	timeOver @7 :Judgement.TimeOver;
	matchClose @8 :Judgement.MatchClose;
	matchAbort @9 :Judgement.MatchAbort;
	playJudgement @10 :Judgement.PlayJudgement;
    }
}

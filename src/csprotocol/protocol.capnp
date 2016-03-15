@0xabe13ea5f0adf862;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Command = import "command.capnp";
using CommonCommand = import "/common/command.capnp";
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
	control @1 :Void;
	bye @2 :Void;
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
	regSuccess @3 :Void;
	regError @4 :Command.RegistrationError;
	fieldOpen @5 :CommonJudgement.FieldOpen;
	kickOff @6 :CommonJudgement.KickOff;
	timeOver @7 :CommonJudgement.TimeOver;
	matchClose @8 :CommonJudgement.MatchClose;
	matchAbort @9 :CommonJudgement.MatchAbort;
	bye @10 :Void;
    }
}

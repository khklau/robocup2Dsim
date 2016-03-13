@0xabe13ea5f0adf862;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Error = import "/common/error.capnp";
using Metadata = import "/common/metadata.capnp";
using Command = import "command.capnp";
using CommonCommand = import "/common/command.capnp";

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
	close @5 :CommonCommand.MatchClose;
	abort @6 :CommonCommand.MatchAbort;
	judgement @7 :Void;
	bye @8 :Void;
    }
}

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
	init @0 :Command.InitRequest;
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
	init @3 :Command.InitReply;
	close @4 :CommonCommand.MatchClose;
	abort @5 :CommonCommand.MatchAbort;
	judgement @6 :Void;
	bye @7 :Void;
    }
}

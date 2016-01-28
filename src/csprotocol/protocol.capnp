@0xabe13ea5f0adf862;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Error = import "/common/error.capnp";
using Metadata = import "/common/metadata.capnp";
using Command = import "command.capnp";
using CommonCommand = import "/common/command.capnp";

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
    }
}

struct ServerStatus
{
    snapshot @0 :AnyPointer;
}

struct ServerTransaction
{
    union
    {
	unknownMsg @0 :Error.UnknownMsgError;
	malformedMsg @1 :Error.MalformedMsgError;
	init @2 :Command.InitReply;
	close @3 :CommonCommand.MatchClose;
	abort @4 :CommonCommand.MatchAbort;
	judgement @5 :Void;
	bye @6 :Void;
    }
}

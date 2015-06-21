@0xca7b62e1bed2bc72;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Metadata = import "/src/core/metadata.capnp";
using Command = import "command.capnp";

struct RequestHeader
{
    sequence @0 :Metadata.SequenceNumber;
}

struct Request
{
    header @0 :RequestHeader;
    union
    {
	init @1 :Command.InitRequest;
	reconnect @2 :Command.ReconnectRequest;
	bye @3 :Command.ByeRequest;
    }
}

struct ReplyHeader
{
    sequence @0 :Metadata.SequenceNumber;
}

struct Reply
{
    header @0 :ReplyHeader;
    union
    {
	init @1 :Command.InitReply;
	reconnect @2 :Command.ReconnectReply;
	bye @3 :Command.ByeReply;
    }
}

@0xca7b62e1bed2bc72;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Metadata = import "/src/core/metadata.capnp";
using Command = import "/src/bcprotocol/command.capnp";

struct Request
{
    union
    {
	init @1 :Command.InitRequest;
	reconnect @2 :Command.ReconnectRequest;
	bye @3 :Command.ByeRequest;
    }
}

struct Reply
{
    union
    {
	init @1 :Command.InitReply;
	reconnect @2 :Command.ReconnectReply;
	bye @3 :Command.ByeReply;
    }
}

struct TransmissionHeader
{
    wantReliable @0 :Bool;
    wantOrdering @1 :Bool;
    channel @1 :Metadata.channelId;
    sequence @2 :Metadata.SequenceNumber;
}

struct ServerTransmission
{
    header @0 :TransmissionHeader
}

struct ClientTransmission
{
    header @0 :TransmissionHeader
}

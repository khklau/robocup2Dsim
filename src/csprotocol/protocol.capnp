@0xca7b62e1bed2bc72;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Error = import "/core/error.capnp";
using Metadata = import "/core/metadata.capnp";
using Command = import "command.capnp";

struct TransmissionHeader
{
    wantReliable @0 :Bool;
    wantOrdering @1 :Bool;
    unusedA @2 :Bool;
    unusedB @3 :Bool;
    unusedC @4 :Bool;
    unusedD @5 :Bool;
    unusedE @6 :Bool;
    unusedF @7 :Bool;
    channel @8 :Metadata.channelId;
    sequence @9 :Metadata.SequenceNumber;
}

struct ClientTransmission
{
    header @0 :TransmissionHeader
    union
    {
	init @1 :Command.InitRequest;
	reconnect @2 :Command.ReconnectRequest;
	bye @3 :Command.ByeRequest;
    }
}

struct ServerTransmission
{
    header @0 :TransmissionHeader
    union
    {
	unknownMsg @1 :Error.UnknownMsgError;
	malformedMsg @2 :Error.MalformedMsgError;
	init @3 :Command.InitReply;
	reconnect @4 :Command.ReconnectReply;
	bye @5 :Command.ByeReply;
    }
}

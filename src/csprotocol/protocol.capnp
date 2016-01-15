@0xabe13ea5f0adf862;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::csprotocol");

using Error = import "/common/error.capnp";
using Metadata = import "/common/metadata.capnp";
using Command = import "command.capnp";
using CommonCommand = import "/common/command.capnp";

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
    channel @8 :Metadata.ChannelId;
    sequence @9 :Metadata.SequenceNumber;
}

struct ClientTransmission
{
    header @0 :TransmissionHeader;
    union
    {
	init @1 :Command.InitRequest;
	control @2 :Void;
    }
}

struct ServerTransmission
{
    header @0 :TransmissionHeader;
    union
    {
	unknownMsg @1 :Error.UnknownMsgError;
	malformedMsg @2 :Error.MalformedMsgError;
	init @3 :Command.InitReply;
	close @4 :CommonCommand.MatchClose;
	abort @5 :CommonCommand.MatchAbort;
    }
}

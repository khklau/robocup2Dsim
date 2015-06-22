@0xca7b62e1bed2bc72;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Command = import "command.capnp";

struct BotTransmission
{
    union
    {
	init @0 :Command.InitRequest;
	reconnect @1 :Command.ReconnectRequest;
	bye @2 :Command.ByeRequest;
    }
}

struct ClientTransmission
{
    union
    {
	init @0 :Command.InitReply;
	reconnect @1 :Command.ReconnectReply;
	bye @2 :Command.ByeReply;
    }
}

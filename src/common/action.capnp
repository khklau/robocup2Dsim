@0xb4dcc02a0871b334;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

using Metadata = import "metadata.capnp";
using Physics = import "physics.capnp";

const velocityScaleFactor :Float32 = 1000.0;

struct MoveFootAction
{
    const maxVelocity :Physics.MetresPerHour = 32000;
    const maxCost :Physics.KiloJoule = 100;
    velocity @0 :Physics.MetresPerHour;
}

struct CatchAction
{
    velocity @0 :Physics.MetresPerHour;
}

struct RunAction
{
    const maxForwardVelocity :Physics.MetresPerHour = 16000;
    const maxForwardCost :Physics.KiloJoule = 100;
    const maxBackwardVelocity :Physics.MetresPerHour = -8000;
    const maxBackwardCost :Physics.KiloJoule = 200;
    velocity @0 :Physics.MetresPerHour;
}

struct TurnHeadAction
{
    const maxVelocity :Physics.DegreesPerSecond = 100;
    const maxCost :Physics.KiloJoule = 20;
    velocity @0 :Physics.DegreesPerSecond;
}

struct TurnTorsoAction
{
    const maxVelocity :Physics.DegreesPerSecond = 200;
    const maxCost :Physics.KiloJoule = 40;
    velocity @0 :Physics.DegreesPerSecond;
}

struct PlayerAction
{
    action :union
    {
	moveFoot @0 :MoveFootAction;
	catch @1 :CatchAction;
	run @2 :RunAction;
	turnHead @3 :TurnHeadAction;
	turnTorso @4 :TurnTorsoAction;
    }
    frame @5 :Metadata.FrameNumber = 0;
}

@0xb4dcc02a0871b334;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

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
    velocity @0 :Physics.DegreesPerSecond;
}

struct TurnTorsoAction
{
    velocity @0 :Physics.DegreesPerSecond;
}

struct PlayerAction
{
    union
    {
	moveFoot @0 :MoveFootAction;
	catch @1 :CatchAction;
    }
}

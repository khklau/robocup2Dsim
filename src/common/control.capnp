@0xb4dcc02a0871b334;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

using Physics = import "physics.capnp";

struct CatchAction
{
    direction @0 :Physics.Degree;
}

struct KickAction
{
    power @0 :Physics.Newton;
    direction @1 :Physics.Degree;
}

struct Control
{
    union
    {
	catch @0 :CatchAction;
	kick @1 :KickAction;
    }
}

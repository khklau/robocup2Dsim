@0xefbea916e22f4681;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Physics = import "/common/physics.capnp";

struct CatchAction
{
    direction @0 :Physics.Degree;
}

struct DashAction
{
    power @0 :Physics.Newton;
}

struct KickAction
{
    power @0 :Physics.Newton;
    direction @1 :Physics.Degree;
}

struct TurnBodyAction
{
    direction @0 :Physics.Degree;
}

struct TurnHeadAction
{
    direction @0 :Physics.Degree;
}

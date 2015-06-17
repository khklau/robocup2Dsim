@0xe713bdb71d3436cd;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

enum PlayMode
{
    beforeKickOff @0;
    timeOver @1;
    playOn @2;
    kickOff @3;
    kickIn @4;
    freeKick @5;
    cornerKick @6;
    goalKick @7;
    afterGoal @8;
    dropBall @9;
    offSide @10;
}

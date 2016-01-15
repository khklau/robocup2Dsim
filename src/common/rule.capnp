@0xe713bdb71d3436cd;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::common");

using Physics = import "/common/physics.capnp";

using GoalCount = UInt8;

struct Score
{
    ourScore @0 :GoalCount;
    theirScore @1 :GoalCount;
}

enum PlayMode
{
    beforeKickOff @0;
    timeOver @1;
    playOn @2;
    kickOff @3;
    throwIn @4;
    freeKick @5;
    cornerKick @6;
    goalKick @7;
    penaltyKick @8;
    afterGoal @9;
    dropBall @10;
    offSide @11;
    onBench @12;
}

enum PlaySession
{
    regular1stHalf @0;
    regular2ndHalf @1;
}

@0xc6f85ffb87e200e2;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::core");

using Entity = import "entity.capnp";
using Metadata = import "metadata.capnp";

struct PlayerInit
{
    version @0 :Metadata.Version;
    teamName @1 :Text;
    uniform @2 :Entity.UniformNumber;
    playerType @3 :Entity.PlayerType;
}

@0xe66422e42c156bb3;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("robocup2Dsim::bcprotocol");

using Metadata = import "/src/core/metadata.capnp";

struct InitRequest
{
    teamName @0 :Text;
    version @1 :Metadata.Version;
    isGoalie @2 :Bool;
}

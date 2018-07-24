#ifndef ROBOCUP2DSIM_COMMON_METADATA_HPP
#define ROBOCUP2DSIM_COMMON_METADATA_HPP

#include <robocup2Dsim/common/metadata.capnp.h>
#include <beam/internet/endpoint.hpp>
#include <limits>
#include <cstdint>

namespace robocup2Dsim {
namespace common {

/**
 * IMPORTANT
 * - these typedefs needs to be kept in sync with metadata.capnp
 * - unfortunately Capn Proto doesn't export its schema typedefs into the generated header
 */
typedef std::uint32_t frame_number;
static const beam::internet::endpoint_id referee_endpoint((127 << 24) + 1, std::numeric_limits<std::uint16_t>::max());

namespace metadata {

inline bool operator==(const Version::Reader& left, const Version::Reader& right)
{
    return left.getNumberA() == right.getNumberA()
	    && left.getNumberB() == right.getNumberB()
	    && left.getNumberC() == right.getNumberC()
	    && left.getNumberD() == right.getNumberD();
}

inline bool operator!=(const Version::Reader& left, const Version::Reader& right)
{
    return !(left == right);
}

} // namespace metadata
} // namespace common
} // namespace robocup2Dsim

#endif

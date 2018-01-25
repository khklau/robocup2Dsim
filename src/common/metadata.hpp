#ifndef ROBOCUP2DSIM_COMMON_METADATA_HPP
#define ROBOCUP2DSIM_COMMON_METADATA_HPP

#include <cstdint>

namespace robocup2Dsim {
namespace common {

/**
 * IMPORTANT
 * - these typedefs needs to be kept in sync with metadata.capnp
 * - unfortunately Capn Proto doesn't export its schema typedefs into the generated header
 */
typedef std::uint32_t frame_number;

} // namespace common
} // namespace robocup2Dsim

#endif

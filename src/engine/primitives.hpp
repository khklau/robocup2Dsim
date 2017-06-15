#ifndef ROBOCUP2DSIM_ENGINE_PRIMITIVES_HPP
#define ROBOCUP2DSIM_ENGINE_PRIMITIVES_HPP

#include <cstddef>
#include <cstdint>
#include <array>

namespace robocup2Dsim {
namespace engine {

typedef std::uint16_t key_16;
typedef std::uint32_t key_32;
typedef std::uint64_t key_64;

template <std::size_t length>
using fixed_cstring = std::array<char, length>;

typedef fixed_cstring<32U> fixed_cstring_32;

} // namespace engine
} // namespace robocup2Dsim

#endif

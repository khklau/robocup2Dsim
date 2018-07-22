#ifndef ROBOCUP2DSIM_REFEREE_CONFIG_HPP
#define ROBOCUP2DSIM_REFEREE_CONFIG_HPP

#include <cstdint>

namespace robocup2Dsim {
namespace referee {

struct config
{
    config() :
	    msg_word_length(64U),
	    msg_buffer_capacity(64U)
    { }
    std::uint16_t msg_word_length;
    std::uint16_t msg_buffer_capacity;
};

} // namespace referee
} // namespace robocup2Dsim

#endif

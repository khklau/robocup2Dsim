#ifndef ROBOCUP2DSIM_ENGINE_PRIMITIVES_HXX
#define ROBOCUP2DSIM_ENGINE_PRIMITIVES_HXX

#include <robocup2Dsim/engine/primitives.hpp>
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace robocup2Dsim {
namespace engine {

template <std::size_t length_c>
fixed_cstring<length_c>::fixed_cstring()
{
    data_[0U] = '\0';
}

template <std::size_t length_c>
fixed_cstring<length_c>::fixed_cstring(const char* other)
{
    assign(other);
}

template <std::size_t length_c>
fixed_cstring<length_c>::fixed_cstring(const std::string& other)
{
    assign(other);
}

template <std::size_t length_c>
fixed_cstring<length_c>::fixed_cstring(const fixed_cstring& other)
{
    assign(other.c_str());
}

template <std::size_t length_c>
fixed_cstring<length_c>& fixed_cstring<length_c>::operator=(const fixed_cstring& other)
{
    if (this != &other)
    {
	this->assign(other.c_str());
    }
    return *this;
}

template <std::size_t length_c>
void fixed_cstring<length_c>::assign(const char* other)
{
    std::size_t other_length = strlen(other);
    if (length_c < (other_length + 1U))
    {
	throw std::invalid_argument("Other string argument is too long");
    }
    else
    {
	::strncpy(data_.data(), other, data_.max_size() - 1U);
    }
}

template <std::size_t length_c>
void fixed_cstring<length_c>::assign(const std::string& other)
{
    std::size_t other_length = other.length();
    if (length_c < (other_length + 1U))
    {
	throw std::invalid_argument("Other string argument is too long");
    }
    else
    {
	::strncpy(data_.data(), other.c_str(), data_.max_size() - 1U);
    }
}

} // namespace engine
} // namespace robocup2Dsim

#endif

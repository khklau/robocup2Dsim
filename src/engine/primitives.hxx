#ifndef ROBOCUP2DSIM_ENGINE_PRIMITIVES_HXX
#define ROBOCUP2DSIM_ENGINE_PRIMITIVES_HXX

#include <robocup2Dsim/engine/primitives.hpp>
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace robocup2Dsim {
namespace engine {
namespace primitives {

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

template <std::size_t this_length_c>
template <std::size_t other_length_c>
fixed_cstring<this_length_c>::fixed_cstring(const char other[other_length_c])
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
bool fixed_cstring<length_c>::operator==(const fixed_cstring& other) const
{
    return ::strncmp(this->c_str(), other.c_str(), length_c) == 0;
}

template <std::size_t length_c>
void fixed_cstring<length_c>::assign(const char* other)
{
    std::size_t other_length = ::strlen(other);
    if (length_c < (other_length + 1U))
    {
	throw std::invalid_argument("Other string argument is too long");
    }
    else
    {
	::strncpy(data_.data(), other, data_.max_size() - 1U);
    }
}

template <std::size_t this_length_c>
template <std::size_t other_length_c>
void fixed_cstring<this_length_c>::assign(const char other[other_length_c])
{
    if (this_length_c < other_length_c)
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

} // namespace primitives
} // namespace engine
} // namespace robocup2Dsim

namespace std {

template <std::size_t length_c>
std::size_t hash<robocup2Dsim::engine::primitives::fixed_cstring<length_c>>::operator()(
	const robocup2Dsim::engine::primitives::fixed_cstring<length_c>& cstring) const
{
    // use the same hash function as java.lang.String
    std::size_t result = 0U;
    for (std::size_t counter = 0U; counter < length_c; ++counter)
    {
	result = cstring.c_str()[counter] + (result * 31U);
    }
    return result;
}

} // namespace std

#endif

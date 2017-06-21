#ifndef ROBOCUP2DSIM_ENGINE_PRIMITIVES_HPP
#define ROBOCUP2DSIM_ENGINE_PRIMITIVES_HPP

#include <cstddef>
#include <cstdint>
#include <array>
#include <string>

namespace robocup2Dsim {
namespace engine {

typedef std::uint16_t key_16;
typedef std::uint32_t key_32;
typedef std::uint64_t key_64;

template <std::size_t length_c>
class fixed_cstring
{
private:
    typedef std::array<char, length_c> array_type;
public:
    using iterator = typename array_type::iterator;
    using const_iterator = typename array_type::const_iterator;
    using reverse_iterator = typename array_type::reverse_iterator;
    using const_reverse_iterator = typename array_type::const_reverse_iterator;
    fixed_cstring();
    fixed_cstring(const char* other);
    fixed_cstring(const std::string& other);
    fixed_cstring(const fixed_cstring& other);
    fixed_cstring& operator=(const fixed_cstring& other);
    inline iterator begin() { return data_.begin(); }
    inline iterator end() { return data_.end(); }
    inline const_iterator cbegin() { return data_.cbegin(); }
    inline const_iterator cend() { return data_.cend(); }
    inline reverse_iterator rbegin() { return data_.rbegin(); }
    inline reverse_iterator rend() { return data_.rend(); }
    inline const_reverse_iterator crbegin() { return data_.crbegin(); }
    inline const_reverse_iterator crend() { return data_.crend(); }
    inline std::size_t max_size() { return data_.max_size(); }
    inline const char* c_str() { return data_.data(); }
    void assign(const char* other);
    void assign(const std::string& other);
private:
    array_type data_;
};

typedef fixed_cstring<16U> fixed_cstring_16;
typedef fixed_cstring<32U> fixed_cstring_32;

} // namespace engine
} // namespace robocup2Dsim

#endif

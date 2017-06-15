#ifndef ROBOCUP2DSIM_ENGINE_RAM_DB_HPP
#define ROBOCUP2DSIM_ENGINE_RAM_DB_HPP

#include <tuple>
#include <unordered_map>
#include <turbo/memory/pool.hpp>

namespace robocup2Dsim {
namespace engine {

template <class key_t, class value_t>
class table
{
public:
    typedef key_t key_type;
    typedef value_t value_type;
    table();
    table(std::size_t initial_size);
private:
    typedef std::tuple<key_t, value_t> tuple_type;
    turbo::memory::block_list data_;
    std::unordered_map<key_t, value_t> index_;
};

} // namespace engine
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_ENGINE_RAM_DB_HPP
#define ROBOCUP2DSIM_ENGINE_RAM_DB_HPP

#include <array>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <robocup2Dsim/engine/primitives.hpp>
#include <turbo/memory/pool.hpp>
#include <turbo/toolset/attribute.hpp>

namespace robocup2Dsim {
namespace engine {

enum class emplace_result
{
    success,
    key_already_exists
};

template <class key_t, class... values_t>
class TURBO_SYMBOL_DECL table
{
public:
    typedef key_t key_type;
    typedef std::tuple<key_t, values_t...> row_type;
    static constexpr std::size_t total_columns()
    {
	// the extra 1 is the key column
	return 1U + sizeof...(values_t);
    }
    template <class... column_names_t>
    table(std::size_t initial_size, const char* key_name, column_names_t&&... column_names);
    template <class... column_names_t>
    emplace_result emplace(key_type key, column_names_t&&... column_names);
private:
    typedef std::unordered_map<fixed_cstring_32, std::size_t> column_map_type;
    template <class column_name_t>
    static void populate_column_map(column_map_type& map, std::size_t column_id, column_name_t&& name);
    template <class column_name_t, class... column_names_t>
    static void populate_column_map(column_map_type& map, std::size_t column_id, column_name_t&& name, column_names_t&&... column_names);
    turbo::memory::block_list data_;
    std::unordered_map<key_t, row_type*> index_;
    std::array<fixed_cstring_32, total_columns()> column_names_;
    column_map_type column_map_;
};

struct TURBO_SYMBOL_DECL some_table;

typedef std::unique_ptr<some_table, void (*)(some_table*)> unique_table_ptr;

template <class key_t, class... values_t>
unique_table_ptr make_unique_table(table<key_t, values_t...>* table);

typedef table<robocup2Dsim::engine::key_16, robocup2Dsim::engine::fixed_cstring_32, unique_table_ptr> catalog;

} // namespace engine
} // namespace robocup2Dsim

#endif

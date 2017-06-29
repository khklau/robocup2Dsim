#ifndef ROBOCUP2DSIM_ENGINE_RAM_DB_HXX
#define ROBOCUP2DSIM_ENGINE_RAM_DB_HXX

#include <robocup2Dsim/engine/primitives.hxx>
#include <turbo/memory/block.hpp>
#include <turbo/toolset/extension.hpp>

namespace {

using namespace robocup2Dsim::engine;

template <class key_t, class... values_t>
void delete_unique_table(some_table* something)
{
     auto actual_table = static_cast<table<key_t, values_t...>*>(static_cast<void*>(something));
     delete actual_table;
}

} // anonymous namespace

namespace robocup2Dsim {
namespace engine {

template <class k, class... vs>
template <class... column_names_t>
table<k, vs...>::table(std::size_t initial_size, const char* key_name, column_names_t&&... column_names)
    :
	data_(sizeof(row_type), initial_size),
	index_(),
	column_names_{{ key_name, column_names... }},
	column_map_()
{
    populate_column_map(column_map_, 0U, key_name, column_names...);
    static_assert(sizeof...(vs) == sizeof...(column_names), "The number of column name arguments "
	    "does not match the number of columns in the table");
}

template <class k, class... vs>
template <class... column_names_t>
emplace_result table<k, vs...>::emplace(key_type key, column_names_t&&... column_names)
{
    if (index_.count(key) > 0)
    {
	return emplace_result::key_already_exists;
    }
    row_type* entry = static_cast<row_type*>(data_.allocate());
    if (TURBO_UNLIKELY(entry == nullptr))
    {
	throw turbo::memory::out_of_memory_error("Insufficient free memory to allocate the requested table row");
    }
    else
    {
	new (entry) row_type(key, std::forward<column_names_t>(column_names)...);
	index_.emplace(key, entry);
	return emplace_result::success;
    }
}

template <class k, class... vs>
template <class column_name_t>
void table<k, vs...>::populate_column_map(
	column_map_type& map,
	std::size_t column_id,
	column_name_t&& name)
{
    map.emplace(name, column_id);
}

template <class k, class... vs>
template <class column_name_t, class... column_names_t>
void table<k, vs...>::populate_column_map(
	column_map_type& map,
	std::size_t column_id,
	column_name_t&& name,
	column_names_t&&... column_names)
{
    map.emplace(name, column_id);
    populate_column_map(map, column_id + 1, column_names...);
}

template <class key_t, class... values_t>
unique_table_ptr make_unique_table(table<key_t, values_t...>* table_ptr)
{
    return std::move(unique_table_ptr(static_cast<some_table*>(static_cast<void*>(table_ptr)),
	    delete_unique_table<key_t, values_t...>));
}

} // namespace engine
} // namespace robocup2Dsim

#endif

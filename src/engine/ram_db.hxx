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
template <class... args_t>
table<k, vs...>::table(std::size_t initial_size, const char* key_name, args_t&&... args)
    :
	data_(sizeof(tuple_type), initial_size),
	column_names_{{ key_name, args... }},
	index_()
{
    static_assert(sizeof...(vs) == sizeof...(args), "The number of column name arguments "
	    "does not match the number of columns in the table");
}

template <class k, class... vs>
template <class... args_t>
emplace_result table<k, vs...>::emplace(key_type key, args_t&&... args)
{
    if (index_.count(key) > 0)
    {
	return emplace_result::key_already_exists;
    }
    tuple_type* entry = static_cast<tuple_type*>(data_.allocate());
    if (TURBO_UNLIKELY(entry == nullptr))
    {
	throw turbo::memory::out_of_memory_error("Insufficient free memory to allocate the requested table row");
    }
    else
    {
	new (entry) tuple_type(key, std::forward<args_t>(args)...);
	index_.emplace(key, entry);
	return emplace_result::success;
    }
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

#ifndef ROBOCUP2DSIM_RUNTIME_RAM_DB_HXX
#define ROBOCUP2DSIM_RUNTIME_RAM_DB_HXX

#include <robocup2Dsim/runtime/ram_db.hpp>
#include <limits>
#include <type_traits>
#include <turbo/memory/block.hxx>
#include <turbo/toolset/extension.hpp>
#include <robocup2Dsim/runtime/primitives.hxx>

namespace {

using namespace robocup2Dsim::runtime;

template <class key_t, class... values_t>
void delete_unique_table(some_table* something)
{
     auto actual_table = static_cast<table<key_t, values_t...>*>(static_cast<void*>(something));
     delete actual_table;
}

template <std::size_t arity_c, class element_t, class tuple_t>
inline element_t& access_element(std::true_type, tuple_t& tuple)
{
    return std::get<arity_c>(tuple);
}

template <std::size_t arity_c, class element_t, class tuple_t>
inline element_t& access_element(std::false_type, tuple_t&)
{
    throw unknown_column_error("the type of the column at the given index does not "
	    "match the element type template argument");
}

template <std::size_t arity_c, class element_t, class tuple_t>
struct element_access
{
    static element_t& access(tuple_t& tuple, std::size_t index)
    {
	if (arity_c == index)
	{
	    return access_element<arity_c, element_t, tuple_t>(std::is_same<element_t, typename std::tuple_element<arity_c, tuple_t>::type>(), tuple);
	}
	else
	{
	    return element_access<arity_c - 1U, element_t, tuple_t>::access(tuple, index);
	}
    }
};

template <class element_t, class tuple_t>
struct element_access<0U, element_t, tuple_t>
{
    static inline element_t& access(tuple_t& tuple, std::size_t)
    {
	return access_element<0U, element_t, tuple_t>(std::is_same<element_t, typename std::tuple_element<0U, tuple_t>::type>(), tuple);
    }
};

template <class element_t, class... tuple_types_t>
const element_t& get_element(const std::tuple<tuple_types_t...>& tuple, std::size_t index)
{
    constexpr std::size_t tuple_size = sizeof...(tuple_types_t);
    if (TURBO_UNLIKELY(tuple_size <= index))
    {
	throw unknown_column_error("given index exceeds the tuple's size");
    }
    else
    {
	return element_access<tuple_size - 1U, const element_t, const std::tuple<tuple_types_t...>>::access(tuple, index);
    }
}

template <class element_t, class... tuple_types_t>
element_t& set_element(std::tuple<tuple_types_t...>& tuple, std::size_t index)
{
    constexpr std::size_t tuple_size = sizeof...(tuple_types_t);
    if (TURBO_UNLIKELY(tuple_size <= index))
    {
	throw unknown_column_error("given index exceeds the tuple's size");
    }
    else
    {
	return element_access<tuple_size - 1U, element_t, std::tuple<tuple_types_t...>>::access(tuple, index);
    }
}

} // anonymous namespace

namespace robocup2Dsim {
namespace runtime {

namespace table_iterator {

template <class r, class i, class c>
basic_const_iterator<r, i, c>::basic_const_iterator(index_iterator_type iter, const typename basic_const_iterator<r, i, c>::column_map_type* map)
    :
	iter_(iter),
	map_(map)
{ }

template <class r, class i, class c>
basic_const_iterator<r, i, c>::basic_const_iterator(const basic_const_iterator& other)
    :
	iter_(other.iter_),
	map_(other.map_)
{ }

template <class r, class i, class c>
basic_const_iterator<r, i, c>& basic_const_iterator<r, i, c>::operator=(const basic_const_iterator& other)
{
    if (this != &other)
    {
	this->iter_ = other.iter_;
	this->map_ = other.map_;
    }
    return *this;
}

template <class r, class i, class c>
bool basic_const_iterator<r, i, c>::operator==(const basic_const_iterator& other) const
{
    return this->iter_ == other.iter_;
}

template <class r, class i, class c>
bool basic_const_iterator<r, i, c>::operator!=(const basic_const_iterator& other) const
{
    return !(*this == other);
}

template <class r, class i, class c>
typename basic_const_iterator<r, i, c>::row_type& basic_const_iterator<r, i, c>::operator*()
{
    return *(iter_->second);
}

template <class r, class i, class c>
typename basic_const_iterator<r, i, c>::row_type* basic_const_iterator<r, i, c>::operator->()
{
    return iter_->second;
}

template <class r, class i, class c>
basic_const_iterator<r, i, c>& basic_const_iterator<r, i, c>::operator++()
{
    ++iter_;
    return *this;
}

template <class r, class i, class c>
basic_const_iterator<r, i, c>& basic_const_iterator<r, i, c>::operator++(int)
{
    basic_const_iterator<r, i, c> tmp = *this;
    ++(*this);
    return tmp;
}

template <class r, class i, class c>
template <class column_t>
const column_t& basic_const_iterator<r, i, c>::get_column(const typename basic_const_iterator<r, i, c>::column_map_type::key_type& key) const
{
    if (!map_)
    {
	throw invalid_deference_error("The end iterator cannot be deferenced");
    }
    typename column_map_type::const_iterator result = map_->find(key);
    if (TURBO_UNLIKELY(result == map_->cend()))
    {
	throw unknown_column_error(key.c_str());
    }
    else
    {
	return get_element<column_t>(*(iter_->second), result->second);
    }
}

template <class r, class i, class c>
basic_iterator<r, i, c>::basic_iterator(index_iterator_type iter, const typename basic_iterator<r, i, c>::column_map_type* map)
    :
	base_type(iter, map)
{ }

template <class r, class i, class c>
template <class column_t>
inline column_t& basic_iterator<r, i, c>::set_column(const typename column_map_type::key_type& key)
{
    if (!(this->map_))
    {
	throw invalid_deference_error("The end iterator cannot be deferenced");
    }
    typename column_map_type::const_iterator result = this->map_->find(key);
    if (TURBO_UNLIKELY(result == this->map_->cend()))
    {
	throw unknown_column_error(key.c_str());
    }
    else
    {
	return set_element<column_t>(*(this->iter_->second), result->second);
    }
}

} // namespace table_iterator

template <class k, class... vs>
template <class... column_names_t>
table<k, vs...>::table(std::size_t initial_size, const char* key_name, column_names_t&&... column_names)
    :
	data_(sizeof(row_type), initial_size),
	index_(),
	column_names_{{ key_name, column_names... }},
	column_map_(),
	auto_key_(std::numeric_limits<key_type>::min())
{
    populate_column_map(column_map_, 0U, key_name, column_names...);
    static_assert(sizeof...(vs) == sizeof...(column_names), "The number of column name arguments "
	    "does not match the number of columns in the table");
}

template <class k, class... vs>
typename table<k, vs...>::iterator table<k, vs...>::end()
{
    return iterator(index_.end(), nullptr);
}

template <class k, class... vs>
typename table<k, vs...>::const_iterator table<k, vs...>::cend() const
{
    return const_iterator(index_.cend(), nullptr);
}

template <class k, class... vs>
template <class... column_values_t>
emplace_result table<k, vs...>::emplace(key_type key, column_values_t&&... column_values)
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
	new (entry) row_type(key, std::forward<column_values_t>(column_values)...);
	index_.emplace(key, entry);
	return emplace_result::success;
    }
}

template <class k, class... vs>
template <class... column_values_t>
typename table<k, vs...>::key_type table<k, vs...>::auto_emplace(column_values_t&&... column_values)
{
    if (auto_key_ == std::numeric_limits<key_type>::max())
    {
	throw key_limit_error("The auto incrementing key is at maximum");
    }
    row_type* entry = static_cast<row_type*>(data_.allocate());
    if (TURBO_UNLIKELY(entry == nullptr))
    {
	throw turbo::memory::out_of_memory_error("Insufficient free memory to allocate the requested table row");
    }
    else
    {
	key_type key_used = auto_key_;
	new (entry) row_type(key_used, std::forward<column_values_t>(column_values)...);
	index_.emplace(key_used, entry);
	++auto_key_;
	return key_used;
    }
}

template <class k, class... vs>
typename table<k, vs...>::const_iterator table<k, vs...>::select_row(key_type key) const
{
    return const_iterator(index_.find(key), &column_map_);
}

template <class k, class... vs>
typename table<k, vs...>::iterator table<k, vs...>::update_row(key_type key)
{
    return iterator(index_.find(key), &column_map_);
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

} // namespace runtime
} // namespace robocup2Dsim

#endif

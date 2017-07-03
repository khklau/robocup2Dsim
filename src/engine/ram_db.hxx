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

namespace table_iterator {

template <class r, class i, class c>
basic_iterator<r, i, c>::basic_iterator(index_iterator_type iter, const typename basic_iterator<r, i, c>::column_map_type* map)
    :
	iter_(iter),
	map_(map)
{ }

template <class r, class i, class c>
basic_iterator<r, i, c>::basic_iterator(const basic_iterator& other)
    :
	iter_(other.iter_),
	map_(other.map_)
{ }

template <class r, class i, class c>
basic_iterator<r, i, c>& basic_iterator<r, i, c>::operator=(const basic_iterator& other)
{
    if (this != &other)
    {
	this->iter_ = other.iter_;
	this->map_ = other.map_;
    }
    return *this;
}

template <class r, class i, class c>
bool basic_iterator<r, i, c>::operator==(const basic_iterator& other) const
{
    return this->iter_ == other.iter_;
}

template <class r, class i, class c>
typename basic_iterator<r, i, c>::row_type& basic_iterator<r, i, c>::operator*()
{
    return *(iter_->second);
}

template <class r, class i, class c>
typename basic_iterator<r, i, c>::row_type* basic_iterator<r, i, c>::operator->()
{
    return iter_->second;
}

template <class r, class i, class c>
basic_iterator<r, i, c>& basic_iterator<r, i, c>::operator++()
{
    ++iter_;
    return *this;
}

template <class r, class i, class c>
basic_iterator<r, i, c>& basic_iterator<r, i, c>::operator++(int)
{
    basic_iterator<r, i, c> tmp = *this;
    ++(*this);
    return tmp;
}

template <class r, class i, class c>
template <class column_t>
inline column_t& basic_iterator<r, i, c>::column(const typename basic_iterator<r, i, c>::column_map_type::key_type& key)
{
    typename column_map_type::const_iterator result = map_->find(key);
    if (TURBO_UNLIKELY(result == map_->cend()))
    {
	throw unknown_column_error(key.c_str());
    }
    else
    {
	switch (result->second)
	{
	    case 0:
		return std::get<0>(*(iter_->second));
	    case 1:
		return std::get<1>(*(iter_->second));
	    case 2:
		return std::get<2>(*(iter_->second));
	    case 3:
		return std::get<3>(*(iter_->second));
	    case 4:
		return std::get<4>(*(iter_->second));
	    case 5:
		return std::get<5>(*(iter_->second));
	    case 6:
		return std::get<6>(*(iter_->second));
	    case 7:
		return std::get<7>(*(iter_->second));
	    case 8:
		return std::get<8>(*(iter_->second));
	    case 9:
		return std::get<9>(*(iter_->second));
	    case 10:
		return std::get<10>(*(iter_->second));
	    case 11:
		return std::get<11>(*(iter_->second));
	    case 12:
		return std::get<12>(*(iter_->second));
	    case 13:
		return std::get<13>(*(iter_->second));
	    case 14:
		return std::get<14>(*(iter_->second));
	    case 15:
		return std::get<15>(*(iter_->second));
	    default:
		throw column_limit_error("column number limit exceeded");
	}
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
typename table<k, vs...>::const_iterator table<k, vs...>::select_row(key_type key) const
{
    return index_.find(key);
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

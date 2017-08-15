#ifndef ROBOCUP2DSIM_RUNTIME_RAM_DB_HPP
#define ROBOCUP2DSIM_RUNTIME_RAM_DB_HPP

#include <cstddef>
#include <array>
#include <iterator>
#include <memory>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <robocup2Dsim/runtime/primitives.hpp>
#include <turbo/memory/block.hpp>
#include <turbo/toolset/attribute.hpp>

namespace robocup2Dsim {
namespace runtime {

static const std::size_t TABLE_COLUMN_NUMBER_LIMIT_ = 16U;

struct unknown_column_error : public std::invalid_argument
{
    explicit unknown_column_error(const char* what) : invalid_argument(what) { }
    explicit unknown_column_error(const std::string& what) : invalid_argument(what) { }
};

struct column_limit_error : public std::invalid_argument
{
    explicit column_limit_error(const char* what) : invalid_argument(what) { }
    explicit column_limit_error(const std::string& what) : invalid_argument(what) { }
};

struct invalid_deference_error : public std::logic_error
{
    explicit invalid_deference_error(const char* what) : logic_error(what) { }
    explicit invalid_deference_error(const std::string& what) : logic_error(what) { }
};

namespace table_iterator {

template <class row_t, class index_iterator_t, class column_map_t>
class basic_const_iterator : public std::forward_iterator_tag
{
public:
    typedef row_t value_type;
    typedef row_t* pointer;
    typedef row_t& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;
    typedef row_t row_type;
    typedef index_iterator_t index_iterator_type;
    typedef column_map_t column_map_type;
    inline basic_const_iterator(index_iterator_type iter, const column_map_type* map);
    inline basic_const_iterator(const basic_const_iterator& other);
    ~basic_const_iterator() = default;
    inline basic_const_iterator& operator=(const basic_const_iterator& other);
    inline bool operator==(const basic_const_iterator& other) const;
    inline bool operator!=(const basic_const_iterator& other) const;
    inline row_type& operator*();
    inline row_type* operator->();
    inline basic_const_iterator& operator++();
    inline basic_const_iterator& operator++(int);
    template <class column_t>
    const column_t& get_column(const typename column_map_type::key_type& key) const;
protected:
    index_iterator_type iter_;
    const column_map_type* map_;
};

template <class row_t, class index_iterator_t, class column_map_t>
class basic_iterator : public basic_const_iterator<row_t, index_iterator_t, column_map_t>
{
public:
    typedef basic_const_iterator<row_t, index_iterator_t, column_map_t> base_type;
    typedef row_t value_type;
    typedef row_t* pointer;
    typedef row_t& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;
    typedef row_t row_type;
    typedef index_iterator_t index_iterator_type;
    typedef column_map_t column_map_type;
    inline basic_iterator(index_iterator_type iter, const column_map_type* map);
    ~basic_iterator() = default;
    template <class column_t>
    column_t& set_column(const typename column_map_type::key_type& key);
};

} // namespace table_iterator

enum class emplace_result
{
    success,
    key_already_exists
};

template <class key_t, class... values_t>
class TURBO_SYMBOL_DECL table
{
private:
    typedef std::tuple<key_t, values_t...> row_type;
    typedef std::unordered_map<key_t, row_type*> row_index_type;
    typedef std::unordered_map<primitives::fixed_cstring_32, std::size_t> column_map_type;
public:
    typedef key_t key_type;
    typedef table_iterator::basic_iterator<row_type, typename row_index_type::iterator, column_map_type> iterator;
    typedef table_iterator::basic_const_iterator<const row_type, typename row_index_type::const_iterator, column_map_type> const_iterator;
    static constexpr std::size_t total_columns()
    {
	// the extra 1 is the key column
	return 1U + sizeof...(values_t);
    }
    template <class... column_names_t>
    table(std::size_t initial_size, const char* key_name, column_names_t&&... column_names);
    inline iterator end();
    inline const_iterator cend() const;
    template <class... column_names_t>
    emplace_result emplace(key_type key, column_names_t&&... column_names);
    inline const_iterator select_row(key_type key) const;
    inline iterator update_row(key_type key);
private:
    template <class column_name_t>
    static void populate_column_map(column_map_type& map, std::size_t column_id, column_name_t&& name);
    template <class column_name_t, class... column_names_t>
    static void populate_column_map(column_map_type& map, std::size_t column_id, column_name_t&& name, column_names_t&&... column_names);
    turbo::memory::block_list data_;
    row_index_type index_;
    std::array<primitives::fixed_cstring_32, total_columns()> column_names_;
    column_map_type column_map_;
};

struct TURBO_SYMBOL_DECL some_table;

typedef std::unique_ptr<some_table, void (*)(some_table*)> unique_table_ptr;

template <class key_t, class... values_t>
unique_table_ptr make_unique_table(table<key_t, values_t...>* table);

typedef table<robocup2Dsim::runtime::primitives::key_16, robocup2Dsim::runtime::primitives::fixed_cstring_32, unique_table_ptr> catalog;

} // namespace runtime
} // namespace robocup2Dsim

#endif

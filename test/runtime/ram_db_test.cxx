#include <robocup2Dsim/runtime/ram_db.hpp>
#include <robocup2Dsim/runtime/ram_db.hxx>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <robocup2Dsim/runtime/primitives.hpp>
#include <robocup2Dsim/runtime/primitives.hxx>

namespace rru = robocup2Dsim::runtime;
namespace rrp = robocup2Dsim::runtime::primitives;

struct coordinate
{
    inline coordinate(std::int32_t, std::int32_t);
    inline coordinate(const coordinate& other);
    inline coordinate& operator=(const coordinate& other);
    inline bool operator==(const coordinate& other) const;
    std::int32_t x;
    std::int32_t y;
};

coordinate::coordinate(std::int32_t a_x, std::int32_t a_y)
    :
	x(a_x),
	y(a_y)
{ }

coordinate::coordinate(const coordinate& other)
    :
	x(other.x),
	y(other.y)
{ }

coordinate& coordinate::operator=(const coordinate& other)
{
    if (this != &other)
    {
	this->x = other.x;
	this->y = other.y;
    }
    return *this;
}

bool coordinate::operator==(const coordinate& other) const
{
    return this->x == other.x && this->y == other.y;
}

typedef rru::table<rrp::key_32, rrp::key_16, coordinate, rrp::fixed_cstring_32> vertex_table_type;

TEST(ram_db_test, select_invalid)
{
    vertex_table_type table1(4U, "vertex_id", "entity_id", "coordinate", "vertex_name");
    auto iter1 = table1.select_row(0U);
    ASSERT_EQ(table1.cend(), iter1) << "Selection of non-existent row succeeded";
}

TEST(ram_db_test, emplace_invalid)
{
    coordinate coordinate1a(5, 10);
    coordinate coordinate1b(14, 9);
    rrp::fixed_cstring_32 vertex_name1a("top_left");
    rrp::fixed_cstring_32 vertex_name1b("top_right");
    vertex_table_type table1(4U, "vertex_id", "entity_id", "coordinate", "vertex_name");
    ASSERT_EQ(rru::emplace_result::success, table1.emplace(0U, 1U, coordinate1a, vertex_name1a)) << "Emplace failed";
    ASSERT_EQ(rru::emplace_result::key_already_exists, table1.emplace(0U, 5U, coordinate1b, vertex_name1b))
	    << "Emplace succeeded with a duplicate key";
}

TEST(ram_db_test, emplace_basic)
{
    coordinate coordinate1(5, 10);
    rrp::fixed_cstring_32 vertex_name1("top_left");
    vertex_table_type table1(4U, "vertex_id", "entity_id", "coordinate", "vertex_name");
    ASSERT_EQ(rru::emplace_result::success, table1.emplace(0U, 1U, coordinate1, vertex_name1)) << "Emplace failed";
    auto iter1 = table1.select_row(0U);
    ASSERT_NE(table1.cend(), iter1) << "Could not select row that was just inserted";
    ASSERT_EQ(0U, iter1.select_column<rrp::key_32>("vertex_id"))
	    << "Column value selected is not the value inserted";
    ASSERT_EQ(1U, iter1.select_column<rrp::key_16>("entity_id"))
	    << "Column value selected is not the value inserted";
    ASSERT_EQ(coordinate1, iter1.select_column<coordinate>("coordinate"))
	    << "Column value selected is not the value inserted";
    ASSERT_TRUE(::strncmp(vertex_name1.c_str(), iter1.select_column<rrp::fixed_cstring_32>("vertex_name").c_str(), vertex_name1.max_size()) == 0)
	    << "Column value selected is not the value inserted";
}

TEST(ram_db_test, update_invalid)
{
    vertex_table_type table1(4U, "vertex_id", "entity_id", "coordinate", "vertex_name");
    auto iter1 = table1.update_row(0U);
    ASSERT_EQ(table1.end(), iter1) << "Selection of non-existent row succeeded";
}

TEST(ram_db_test, update_basic)
{
    coordinate coordinate1a(5, 10);
    coordinate coordinate1b(14, 9);
    rrp::fixed_cstring_32 vertex_name1a("top_left");
    rrp::fixed_cstring_32 vertex_name1b("top_right");
    vertex_table_type table1(4U, "vertex_id", "entity_id", "coordinate", "vertex_name");
    ASSERT_EQ(rru::emplace_result::success, table1.emplace(0U, 1U, coordinate1a, vertex_name1a)) << "Emplace failed";
    auto iter1 = table1.update_row(0U);
    iter1.update_column<rrp::key_16>("entity_id") = 5U;
    iter1.update_column<coordinate>("coordinate") = coordinate1b;
    iter1.update_column<rrp::fixed_cstring_32>("vertex_name").assign(vertex_name1b.c_str());
    ASSERT_NE(table1.end(), iter1) << "Could not select row that was just inserted";
    ASSERT_EQ(0U, iter1.select_column<rrp::key_32>("vertex_id"))
	    << "Column value selected is not the value updated";
    ASSERT_EQ(5U, iter1.select_column<rrp::key_16>("entity_id"))
	    << "Column value selected is not the value updatd";
    ASSERT_EQ(coordinate1b, iter1.select_column<coordinate>("coordinate"))
	    << "Column value selected is not the value updated";
    ASSERT_TRUE(::strncmp(vertex_name1b.c_str(), iter1.select_column<rrp::fixed_cstring_32>("vertex_name").c_str(), vertex_name1b.max_size()) == 0)
	    << "Column value selected is not the value updated";
}

#include <robocup2Dsim/engine/ram_db.hpp>
#include <robocup2Dsim/engine/ram_db.hxx>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <robocup2Dsim/engine/primitives.hpp>
#include <robocup2Dsim/engine/primitives.hxx>

namespace ren = robocup2Dsim::engine;

struct coordinate
{
    inline coordinate(std::int32_t, std::int32_t);
    inline coordinate(const coordinate& other);
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

bool coordinate::operator==(const coordinate& other) const
{
    return this->x == other.x && this->y == other.y;
}

typedef ren::table<ren::key_32, ren::key_16, coordinate, ren::fixed_cstring_32> vertex_table_type;

TEST(ram_db_test, emplace_basic)
{
    coordinate coordinate1(5, 10);
    ren::fixed_cstring_32 vertex_name1("top_left");
    vertex_table_type table1(4U, "vertex_id", "entity_id", "coordinate", "vertex_name");
    ASSERT_EQ(ren::emplace_result::success, table1.emplace(0U, 1U, coordinate1, vertex_name1)) << "Emplace failed";
    auto iter1 = table1.select_row(0U);
    ASSERT_NE(table1.cend(), iter1) << "Could not select row that was just inserted";
    ASSERT_EQ(0U, iter1.get_column<ren::key_32>("vertex_id"))
	    << "Column value selected is not the value inserted";
    ASSERT_EQ(1U, iter1.get_column<ren::key_16>("entity_id"))
	    << "Column value selected is not the value inserted";
    ASSERT_EQ(coordinate1, iter1.get_column<coordinate>("coordinate"))
	    << "Column value selected is not the value inserted";
    ASSERT_TRUE(::strncmp(vertex_name1.c_str(), iter1.get_column<ren::fixed_cstring_32>("vertex_name").c_str(), vertex_name1.max_size()) == 0)
	    << "Column value selected is not the value inserted";
}

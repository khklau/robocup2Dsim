#ifndef ROBOCUP2DSIM_COMMON_ECS_DB_HPP
#define ROBOCUP2DSIM_COMMON_ECS_DB_HPP

#include <cstdint>
#include <robocup2Dsim/engine/ram_db.hpp>
#include <turbo/toolset/attribute.hpp>

namespace robocup2Dsim {
namespace common {

namespace table_id {

enum TURBO_SYMBOL_DECL type : std::uint16_t
{
    entity,
    component,
    system
};

} // namespace table_id

class TURBO_SYMBOL_DECL ecs_db
{
public:
    typedef robocup2Dsim::engine::table<robocup2Dsim::engine::key_16, robocup2Dsim::engine::fixed_cstring_32> entity_table_type;
    typedef robocup2Dsim::engine::table<robocup2Dsim::engine::key_16, robocup2Dsim::engine::fixed_cstring_32> component_table_type;
    typedef robocup2Dsim::engine::table<robocup2Dsim::engine::key_16, robocup2Dsim::engine::fixed_cstring_32> system_table_type;
    ecs_db();
    template <class table_t>
    table_t& access(table_id::type id);
    template <class table_t>
    const table_t& access(table_id::type id) const;
private:
    robocup2Dsim::engine::catalog catalog_;
};

} // namespace common
} // namespace robocup2Dsim

#endif

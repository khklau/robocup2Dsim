#ifndef ROBOCUP2DSIM_ENGINE_ECS_DB_HPP
#define ROBOCUP2DSIM_ENGINE_ECS_DB_HPP

#include <cstdint>
#include <robocup2Dsim/engine/ram_db.hpp>
#include <turbo/toolset/attribute.hpp>

namespace robocup2Dsim {
namespace engine {

namespace table_id {

enum TURBO_SYMBOL_DECL type : std::uint16_t
{
    entity_registry,
    component_registry,
    system_registry,
    entity_component_map,
    component_system_map
};

} // namespace table_id

namespace system_id {

enum TURBO_SYMBOL_DECL type : std::uint16_t
{
    physics
};

} // namespace system_id

class TURBO_SYMBOL_DECL ecs_db
{
public:
    typedef table<primitives::key_16, primitives::fixed_cstring_32> entity_table_type;
    typedef table<primitives::key_16, unique_table_ptr, primitives::fixed_cstring_32> component_table_type;
    typedef table<primitives::key_16, unique_table_ptr, primitives::fixed_cstring_32> system_table_type;
    typedef table<primitives::key_16, primitives::key_16, primitives::key_32> entity_component_table_type;
    typedef table<primitives::key_16, primitives::key_16, primitives::key_32> component_system_table_type;
    ecs_db();
    template <class table_t>
    table_t& access(table_id::type id);
    template <class table_t>
    const table_t& access(table_id::type id) const;
private:
    robocup2Dsim::engine::catalog catalog_;
};

} // namespace engine
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_RUNTIME_ECS_DB_HPP
#define ROBOCUP2DSIM_RUNTIME_ECS_DB_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <robocup2Dsim/runtime/ram_db.hpp>
#include <turbo/memory/slab_allocator.hpp>
#include <turbo/toolset/attribute.hpp>

namespace robocup2Dsim {
namespace runtime {

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

namespace component_id {

enum TURBO_SYMBOL_DECL type : std::uint16_t
{
    body
};

} // namespace component_id

class TURBO_SYMBOL_DECL ecs_db
{
public:
    typedef table<primitives::key_16, primitives::fixed_cstring_32> entity_table_type;
    typedef table<primitives::key_16, primitives::fixed_cstring_32> component_table_type;
    typedef table<primitives::key_16, unique_table_ptr, primitives::fixed_cstring_32> system_table_type;
    typedef table<primitives::key_32, primitives::key_16, primitives::key_16> entity_component_table_type;
    typedef table<primitives::key_32, primitives::key_16, primitives::key_16> component_system_table_type;
    typedef robocup2Dsim::runtime::ecs_db::entity_table_type::key_type entity_id_type;
    ecs_db(std::size_t contingency_capacity, const std::vector<turbo::memory::block_config>& allocator_config);
    template <class table_t>
    table_t& access(table_id::type id);
    template <class table_t>
    const table_t& access(table_id::type id) const;
    entity_id_type insert_entity(const std::string& name);
    inline turbo::memory::concurrent_sized_slab& component_allocator()
    {
	return component_allocator_;
    }
private:
    turbo::memory::concurrent_sized_slab component_allocator_;
    robocup2Dsim::runtime::catalog catalog_;
};

} // namespace runtime
} // namespace robocup2Dsim

#endif

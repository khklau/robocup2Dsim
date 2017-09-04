#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HPP
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HPP

#include <bitset>
#include <memory>
#include <type_traits>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>
#include <turbo/toolset/attribute.hpp>
#include <robocup2Dsim/runtime/primitives.hpp>
#include <robocup2Dsim/runtime/ram_db.hpp>
#include <robocup2Dsim/runtime/resource.hpp>
#include <robocup2Dsim/engine/dynamics.hpp>

namespace robocup2Dsim {

namespace runtime {

class ecs_db;

} // namespace runtime

namespace engine {

enum class contact_result
{
    pass_over = 0U,
    collide
};

template <class entity_category_t>
class collision_config
{
public:
    typedef entity_category_t entity_category_type;
    static constexpr std::size_t category_bit_size = std::numeric_limits<decltype(b2Filter::categoryBits)>::digits;
    static_assert(std::is_unsigned<typename std::underlying_type<entity_category_type>::type>::value,
	    "entity_cateogry_type template parameter must be have an underlying type that is an unsigned integer");
    static_assert(std::numeric_limits<typename std::underlying_type<entity_category_type>::type>::digits <= category_bit_size,
	    "the underlying type of the entity_cateogry_type template exceeds the supported size");
    collision_config(contact_result initial);
    ~collision_config() = default;
    contact_result get(entity_category_type category) const;
    void set(entity_category_type category, contact_result contact);
private:
    std::bitset<category_bit_size> config_;
};

template <class element_t>
using physics_ptr = robocup2Dsim::runtime::borrowed_ptr<robocup2Dsim::runtime::system_id::physics, element_t>;

class TURBO_SYMBOL_DECL physics
{
public:
    typedef b2Fixture fixture;
    physics();
private:
    b2World world_;
};

typedef robocup2Dsim::runtime::table<
		robocup2Dsim::runtime::primitives::key_16,
		std::unique_ptr<physics>,
		robocup2Dsim::runtime::primitives::fixed_cstring_32>
	physics_table_type;

void register_system(robocup2Dsim::runtime::ecs_db& db, std::unique_ptr<physics> phys);

} // namespace engine
} // namespace robobup2Dsim

#endif

#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HPP
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HPP

#include <bitset>
#include <memory>
#include <type_traits>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <turbo/toolset/attribute.hpp>
#include <robocup2Dsim/runtime/ecs_db.hpp>
#include <robocup2Dsim/runtime/primitives.hpp>
#include <robocup2Dsim/runtime/ram_db.hpp>
#include <robocup2Dsim/runtime/resource.hpp>
#include <robocup2Dsim/engine/dynamics.hpp>
#include <robocup2Dsim/engine/math.hpp>

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

template <class contact_category_t>
class contact_config
{
public:
    typedef contact_category_t contact_category_type;
    typedef typename std::underlying_type<contact_category_type>::type category_underlying_type;
    static constexpr std::size_t max_category_limit = std::numeric_limits<decltype(b2Filter::categoryBits)>::digits;
    static_assert(std::is_unsigned<category_underlying_type>::value,
	    "entity_cateogry_type template parameter must be have an underlying type that is an unsigned integer");
    static_assert(static_cast<category_underlying_type>(contact_category_type::max) <= max_category_limit,
	    "the maximum value of entity_cateogry_type template exceeds the supported category limit");
    contact_config(contact_result initial, contact_result from_same_entity);
    ~contact_config() = default;
    inline decltype(b2Filter::categoryBits) to_uint() const;
    inline contact_result get_from_same_entity_config() const;
    contact_result get(contact_category_type category) const;
    void set(contact_category_type category, contact_result contact);
private:
    std::bitset<max_category_limit> config_;
    contact_result from_same_entity_;
};

template <class element_t>
using physics_ptr = robocup2Dsim::runtime::borrowed_ptr<robocup2Dsim::runtime::system_id::physics, element_t>;

class TURBO_SYMBOL_DECL physics
{
public:
    typedef b2Vec2 vec2;
    typedef b2BodyDef body_def;
    typedef b2MassData mass_data;
    typedef b2FixtureDef fixture_def;
    typedef b2CircleShape circle_shape;
    typedef b2PolygonShape polygon_shape;
    typedef b2RevoluteJointDef revolute_joint_def;
    typedef b2PrismaticJointDef prismatic_joint_def;
    typedef robocup2Dsim::runtime::ecs_db::entity_table_type::key_type entity_id_type;
    physics();
    physics(const vec2& gravity);
    physics_ptr<dynamics::body> make_body(entity_id_type entity_id, const body_def& def);
    template <class contact_category_t>
    fixture_def make_fixture_def(
	    entity_id_type entity_id,
	    contact_category_t category,
	    const contact_config<contact_category_t>& contact);
    void make_fixture(
	    entity_id_type entity_id,
	    physics_ptr<dynamics::body> body,
	    const fixture_def& def);
    void make_joint(
	    entity_id_type entity_id,
	    const revolute_joint_def& def);
    void make_joint(
	    entity_id_type entity_id,
	    const prismatic_joint_def& def);
private:
    typedef b2Fixture fixture;
    typedef b2RevoluteJoint revolute_joint;
    typedef b2PrismaticJoint prismatic_joint;
    template <class joint_t, class def_t>
    void make_joint(
	    entity_id_type entity_id,
	    const def_t& def);
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

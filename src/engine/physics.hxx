#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HXX
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HXX

#include <utility>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <robocup2Dsim/runtime/resource.hxx>

namespace robocup2Dsim {
namespace engine {

template <class contact_category_t>
contact_config<contact_category_t>::contact_config(
	contact_result initial,
	contact_result from_same_entity)
    :
	config_(initial == contact_result::collide
		? std::numeric_limits<decltype(b2Filter::categoryBits)>::max()
		: std::numeric_limits<decltype(b2Filter::categoryBits)>::min()),
	from_same_entity_(from_same_entity)
{ }

template <class contact_category_t>
decltype(b2Filter::categoryBits) contact_config<contact_category_t>::to_uint() const
{
    return config_.to_ulong();
}

template <class contact_category_t>
contact_result contact_config<contact_category_t>::get_from_same_entity_config() const
{
    return from_same_entity_;
}

template <class contact_category_t>
contact_result contact_config<contact_category_t>::get(contact_category_type category) const
{
    return config_[static_cast<category_underlying_type>(category)]
	? contact_result::collide
	: contact_result::pass_over;
}

template <class contact_category_t>
void contact_config<contact_category_t>::set(contact_category_type category, contact_result contact)
{
    switch (contact)
    {
	case contact_result::collide:
	{
	    config_.set(static_cast<category_underlying_type>(category), true);
	    break;
	}
	case contact_result::pass_over:
	{
	    config_.set(static_cast<category_underlying_type>(category), false);
	    break;
	}
	default:
	{
	    break;
	}
    }
}

template <class contact_category_t>
typename physics::fixture_def physics::make_fixture_def(
	entity_id_type entity_id,
	contact_category_t category,
	const contact_config<contact_category_t>& contact)
{
    fixture_def result;
    result.filter.categoryBits = 1U << static_cast<typename contact_config<contact_category_t>::category_underlying_type>(category);
    result.filter.maskBits = contact.to_uint();
    if (contact.get_from_same_entity_config() == contact_result::pass_over)
    {
	result.filter.groupIndex = entity_id * -1;
    }
    return result;
}

template <class joint_t, class def_t>
void physics::make_joint(
	entity_id_type entity_id,
	const def_t& def)
{
    joint_t* result = static_cast<joint_t*>(world_.CreateJoint(&def));
    result->SetUserData(reinterpret_cast<void*>(static_cast<std::uintptr_t>(entity_id)));
}

namespace {

using namespace robocup2Dsim::engine;

template <typename collision_func_t, typename separation_func_t>
struct contact_listener : public b2ContactListener
{
    typedef collision_func_t collision_func_type;
    typedef separation_func_t separation_func_type;
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    collision_func_t on_collision;
    separation_func_t on_separation;
};

template <typename c, typename s>
void contact_listener<c, s>::BeginContact(b2Contact* contact)
{
    if (contact)
    {
	on_collision(
		static_cast<physics::entity_id_type>(reinterpret_cast<std::uintptr_t>(contact->GetFixtureA()->GetUserData())),
		static_cast<physics::entity_id_type>(reinterpret_cast<std::uintptr_t>(contact->GetFixtureB()->GetUserData())));
    }
}

template <typename c, typename s>
void contact_listener<c, s>::EndContact(b2Contact* contact)
{
    if (contact)
    {
	on_separation(
		static_cast<physics::entity_id_type>(reinterpret_cast<std::uintptr_t>(contact->GetFixtureA()->GetUserData())),
		static_cast<physics::entity_id_type>(reinterpret_cast<std::uintptr_t>(contact->GetFixtureB()->GetUserData())));
    }
}

} // anonyous namespace

template <typename collision_func_t, typename separation_func_t>
void physics::step(float time_step, collision_func_t on_collision, separation_func_t on_separation)
{
    contact_listener<collision_func_t, separation_func_t> listener{on_collision, on_separation};
    world_.SetContactListener(&listener);
    world_.Step(time_step, solver_conf_.velocity_iteration_limit, solver_conf_.position_iteration_limit);
}

} // namespace engine
} // namespace robocup2Dsim

#endif

#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HXX
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HXX

#include <utility>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <robocup2Dsim/runtime/resource.hh>

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

namespace {

using namespace robocup2Dsim::engine;
namespace rru = robocup2Dsim::runtime;

void set_fixture_data(physics::fixture_def& def, rru::ecs_db::entity_id_type entity_id, physics::fixture_id_type fixture_id)
{
    std::uintptr_t data = fixture_id;
    data <<= std::numeric_limits<rru::ecs_db::entity_id_type>::digits;
    data |= entity_id;
    def.userData = reinterpret_cast<void*>(data);
}

rru::ecs_db::entity_id_type get_entity_id(const b2Fixture& fixture)
{
    std::uintptr_t mask = 0U;
    mask |= std::numeric_limits<rru::ecs_db::entity_id_type>::max();
    return reinterpret_cast<std::uintptr_t>(fixture.GetUserData()) & mask;
}

physics::fixture_id_type get_fixture_id(const b2Fixture& fixture)
{
    std::uintptr_t mask = 0U;
    mask |= std::numeric_limits<physics::fixture_id_type>::max();
    std::uintptr_t raw_data = reinterpret_cast<std::uintptr_t>(fixture.GetUserData());
    return (raw_data >> std::numeric_limits<rru::ecs_db::entity_id_type>::digits) & mask;
}

template <typename collision_func_t, typename separation_func_t>
struct contact_listener : public b2ContactListener
{
    typedef collision_func_t collision_func_type;
    typedef separation_func_t separation_func_type;
    contact_listener(collision_func_type&& collision, separation_func_t&& separation);
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    collision_func_t on_collision;
    separation_func_t on_separation;
};

template <typename c, typename s>
contact_listener<c, s>::contact_listener(collision_func_type&& collision, separation_func_type&& separation)
    :
	on_collision(std::forward<collision_func_type>(collision)),
	on_separation(std::forward<separation_func_type>(separation))
{ }

template <typename c, typename s>
void contact_listener<c, s>::BeginContact(b2Contact* contact)
{
    if (contact)
    {
	physics::contact_participant participant_a{
		get_entity_id(*(contact->GetFixtureA())),
		get_fixture_id(*(contact->GetFixtureA())),
		*(contact->GetFixtureA()->GetBody())};
	physics::contact_participant participant_b{
		get_entity_id(*(contact->GetFixtureB())),
		get_fixture_id(*(contact->GetFixtureB())),
		*(contact->GetFixtureB()->GetBody())};
	on_collision(participant_a, participant_b);
    }
}

template <typename c, typename s>
void contact_listener<c, s>::EndContact(b2Contact* contact)
{
    if (contact)
    {
	physics::contact_participant participant_a{
		get_entity_id(*(contact->GetFixtureA())),
		get_fixture_id(*(contact->GetFixtureA())),
		*(contact->GetFixtureA()->GetBody())};
	physics::contact_participant participant_b{
		get_entity_id(*(contact->GetFixtureB())),
		get_fixture_id(*(contact->GetFixtureB())),
		*(contact->GetFixtureB()->GetBody())};
	on_separation(participant_a, participant_b);
    }
}

static constexpr std::uint16_t group_index_offset = 1U;

} // anonyous namespace

template <class contact_category_t>
typename physics::fixture_def physics::make_fixture_def(
	robocup2Dsim::runtime::ecs_db::entity_id_type entity_id,
	fixture_id_type fixture_id,
	contact_category_t category,
	const contact_config<contact_category_t>& contact)
{
    fixture_def result;
    result.filter.categoryBits = 1U << static_cast<typename contact_config<contact_category_t>::category_underlying_type>(category);
    result.filter.maskBits = contact.to_uint();
    if (contact.get_from_same_entity_config() == contact_result::pass_over)
    {
        // to achieve pass over the groupIndex cannot be 0
	result.filter.groupIndex = (entity_id + group_index_offset) * -1;
    }
    set_fixture_data(result, entity_id, fixture_id);
    return result;
}

template <class joint_t, class def_t>
void physics::make_joint(const def_t& def)
{
    joint_t* result = static_cast<joint_t*>(world_.CreateJoint(&def));
    for (const fixture* fix = result->GetBodyA()->GetFixtureList(); fix != nullptr; fix = fix->GetNext())
    {
	if (fix != nullptr)
	{
	    result->SetUserData(reinterpret_cast<void*>(static_cast<std::uintptr_t>(get_entity_id(*fix))));
	    break;
	}
    }
}

template <typename collision_func_t, typename separation_func_t>
void physics::step(float time_step, collision_func_t&& on_collision, separation_func_t&& on_separation)
{
    contact_listener<collision_func_t, separation_func_t> listener(
	    std::forward<collision_func_t>(on_collision),
	    std::forward<separation_func_t>(on_separation));
    world_.SetContactListener(&listener);
    world_.Step(time_step, solver_conf_.velocity_iteration_limit, solver_conf_.position_iteration_limit);
    world_.SetContactListener(nullptr);
}

} // namespace engine
} // namespace robocup2Dsim

#endif

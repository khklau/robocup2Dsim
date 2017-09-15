#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HXX
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HXX

#include <utility>
#include <Box2D/Dynamics/b2Body.h>
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

} // namespace engine
} // namespace robocup2Dsim

#endif

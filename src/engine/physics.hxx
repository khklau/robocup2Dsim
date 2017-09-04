#ifndef ROBOCUP2DSIM_ENGINE_PHYSICS_HXX
#define ROBOCUP2DSIM_ENGINE_PHYSICS_HXX

namespace robocup2Dsim {
namespace engine {

template <class entity_category_t>
collision_config<entity_category_t>::collision_config(contact_result initial)
    :
	config_(initial == contact_result::collide
		? std::numeric_limits<decltype(b2Filter::categoryBits)>::max()
		: std::numeric_limits<decltype(b2Filter::categoryBits)>::min())
{ }

template <class entity_category_t>
contact_result collision_config<entity_category_t>::get(entity_category_type category) const
{
    return config_[category]
	? contact_result::collide
	: contact_result::pass_over;
}

template <class entity_category_t>
void collision_config<entity_category_t>::set(entity_category_type category, contact_result contact)
{
    switch (contact)
    {
	case contact_result::collide:
	{
	    config_.set(category, true);
	    break;
	}
	case contact_result::pass_over:
	{
	    config_.set(category, false);
	    break;
	}
	default:
	{
	    break;
	}
    }
}

} // namespace engine
} // namespace robocup2Dsim

#endif

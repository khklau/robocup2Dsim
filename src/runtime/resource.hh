#ifndef ROBOCUP2DSIM_RUNTIME_RESOURCE_HXX
#define ROBOCUP2DSIM_RUNTIME_RESOURCE_HXX

#include <robocup2Dsim/runtime/resource.hpp>

namespace robocup2Dsim {
namespace runtime {

template <system_id::type o, class e>
borrowed_ptr<o, e>::borrowed_ptr(element_type* ptr) noexcept
    :
	ptr_(ptr)
{ }

template <system_id::type o, class e>
borrowed_ptr<o, e>::borrowed_ptr(const borrowed_ptr& other) noexcept
    :
	ptr_(other.ptr_)
{ }

template <system_id::type o, class e>
borrowed_ptr<o, e>& borrowed_ptr<o, e>::operator=(const borrowed_ptr& other)
{
    if (this != &other)
    {
	ptr_ = other.ptr_;
    }
    return *this;
}

template <system_id::type o, class e>
borrowed_ptr<o, e>::operator bool() const
{
    return ptr_ != nullptr;
}

} // namespace runtime
} // namespace robocup2Dsim

#endif

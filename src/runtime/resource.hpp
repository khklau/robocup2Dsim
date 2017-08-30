#ifndef ROBOCUP2DSIM_RUNTIME_RESOURCE_HPP
#define ROBOCUP2DSIM_RUNTIME_RESOURCE_HPP

#include <robocup2Dsim/runtime/ecs_db.hpp>

namespace robocup2Dsim {
namespace runtime {

template <system_id::type owner_c, class element_t>
class borrowed_ptr
{
public:
    typedef element_t element_type;
    constexpr borrowed_ptr() noexcept
	:
	    ptr_(nullptr)
    { }
    explicit borrowed_ptr(element_type* ptr) noexcept;
    borrowed_ptr(const borrowed_ptr& other) noexcept;
    ~borrowed_ptr() = default;
    borrowed_ptr& operator=(const borrowed_ptr& other);
    explicit operator bool() const;
    inline element_type& operator*()
    {
	return *ptr_;
    }
    inline const element_type& operator*() const
    {
	return *ptr_;
    }
    inline element_type* operator->()
    {
	return ptr_;
    }
    inline const element_type* operator->() const
    {
	return ptr_;
    }
private:
    element_type* ptr_;
};

} // namespace runtime
} // namespace robocup2Dsim

#endif

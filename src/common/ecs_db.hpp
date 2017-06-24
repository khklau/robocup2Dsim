#ifndef ROBOCUP2DSIM_COMMON_ECS_DB_HPP
#define ROBOCUP2DSIM_COMMON_ECS_DB_HPP

#include <robocup2Dsim/engine/ram_db.hpp>
#include <turbo/toolset/attribute.hpp>

namespace robocup2Dsim {
namespace common {

class TURBO_SYMBOL_DECL ecs_db
{
public:
    ecs_db();
private:
    robocup2Dsim::engine::catalog catalog_;
};

} // namespace common
} // namespace robocup2Dsim

#endif

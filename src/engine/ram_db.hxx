#ifndef ROBOCUP2DSIM_ENGINE_RAM_DB_HXX
#define ROBOCUP2DSIM_ENGINE_RAM_DB_HXX

namespace robocup2Dsim {
namespace engine {

template <class k, class v>
table<k, v>::table()
    :
	table(32U)
{ }

template <class k, class v>
table<k, v>::table(std::size_t initial_size)
    :
	data_(sizeof(tuple_type), initial_size),
	index_()
{ }

} // namespace engine
} // namespace robocup2Dsim

#endif

#include "server_io.hpp"
#include <unistd.h>

namespace robocup2Dsim {
namespace referee {

server_io::server_io(const config& conf)
    :
	config_(conf),
	service_(),
	stdin_(service_, ::dup(STDIN_FILENO)),
	stdout_(service_, ::dup(STDOUT_FILENO)),
	pool_(config_.msg_word_length, config_.msg_buffer_capacity)
{ }

server_io::~server_io()
{
    if (!service_.stopped())
    {
	service_.stop();
    }
}

} // namespace referee
} // namespace robocup2Dsim

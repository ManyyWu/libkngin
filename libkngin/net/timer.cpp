#include <sys/timerfd.h>
#include <cstring>
#include "define.h"
#include "timer.h"
#include "filefd.h"
#include "common.h"

__NAMESPACE_BEGIN

timer::timer ()
    : filefd(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK))
{
    if_not (__fd_valid(m_fd)) {
        log_fatal("timerfd_create() error - %s:%d", strerror(errno), errno);
        throw exception("timer::timer() erorr");
    }
}

timer::~timer ()
{
}

void
set_time (const timestamp &_ms, bool _abs /* = false */)
{

}

void
set_time (const timestamp &&_ms, bool _abs /* = false */)
{

}

void
set_interval (const timestamp &_ms)
{

}

void
set_interval (const timestamp &&_ms)
{

}

__NAMESPACE_END

#include <sys/eventfd.h>
#include <cstring>
#include <unistd.h>
#include "define.h"
#include "event_loop.h"
#include "local_lock.h"
#include "thread.h"
#include "file.h"

__NAMESPACE_BEGIN

event_loop::event_loop (thread *_thr)
    try
    : m_waker_fd(event_loop::create_waker()),
      m_thr(_thr),
      m_mutex()
{
    kassert(__fd_valid(m_waker_fd));
    kassert(_thr);

} catch (...) {
    if (__fd_valid(m_waker_fd))
        event_loop::destroy_waker(m_waker_fd);
    throw;
}

event_loop::~event_loop ()
{
    event_loop::destroy_waker(m_waker_fd);
}

int
event_loop::loop (void *_args)
{
    return 0;
}

event_loop::waker
event_loop::create_waker()
{
    event_loop::waker _efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (_efd < 0)
        log_fatal("eventfd() error - %s:%d", strerror(errno), errno);
    return _efd;
}

void
event_loop::destroy_waker (event_loop::waker _waker)
{
    kassert(__fd_valid(_waker));
    ::close(_waker);
}

__NAMESPACE_END

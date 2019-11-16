#ifdef _WIN32
#else
#include <unistd.h>
#include <sys/epoll.h>
#endif
#include <cstring>
#include <map>
#include "common.h"
#include "timestamp.h"
#include "exception.h"
#include "epoller.h"
#include "filefd.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/epoller.cpp"

__NAMESPACE_BEGIN

epoller::epoller (event_loop *_loop)
    try
    :
#ifndef NDEBUG
      m_fd_set(),
      m_mutex(),
#endif
      m_loop(_loop),
      m_epollfd(::epoll_create1(EPOLL_CLOEXEC))
{
    check(_loop);
    if (__fd_invalid(m_epollfd)) {
        log_fatal("::epoll_create1() error - %s:%d", m_epollfd, strerror(errno), errno);
        throw exception("epoller::epoller() error");
    }

    //log_debug("new epoller, fd = %d", m_epollfd);
} catch (...) {
    if (__fd_valid(m_epollfd))
        ::close(m_epollfd);
    throw;
}

epoller::~epoller ()
{
     this->close();
}

uint32_t
epoller::wait (epoller::epoll_event_set &_list, timestamp _ms)
{
    check(__fd_valid(m_epollfd));
    int _num = ::epoll_wait(m_epollfd, _list.data(), (int)_list.size(), (int)_ms.value_int());
    if (_num < 0) {
        if (EINTR == errno)
            return 0;
        log_error("::epoll_wait() error - %s:%d", strerror(errno), errno);
    }
    return std::max(_num, 0);
}

void
epoller::close ()
{
    //log_debug("epoller closed, fd = %d", m_epollfd);

#ifndef NDEBUG
    if (!m_fd_set.empty())
        log_warning("there are still have %" PRIu64 " undeleted fd in epoller", m_fd_set.size());
#endif
    if (__fd_valid(m_epollfd)) {
        if (::close(m_epollfd) < 0)
                    log_error("::close() error - %s:%d", strerror(errno), errno);
        m_epollfd = __INVALID_FD;
    }
}

bool
epoller::update_event (int _opt, epoller_event *_e)
{
    /*
    * NOTES:
    * While one thread is blocked in a call to epoll_pwait(), it is possible for another thread
    * to add a file descriptor to the waited-upon epoller instance. If the new file descriptor
    * becomes ready, it will cause the epoll_wait() call to unblock.
    *
    * If a file descriptor being monitored by select() is closed in another thread, the result
    * is unspecified.  On some UNIX systems, select() unblocks and returns, with an indication
    * that the file descriptor is ready (a subsequent I/O operation will likely fail with an error,
    * unless another the file descriptor reopened between the time select() returned  and the I/O
    * operations was performed).  On Linux (and some other systems), closing the file descriptor
    * in another thread has no effect on select().  In summary, any application that relies on a
    * particular behavior in this scenario must be considered buggy.
    */
    check(_e);

    int _fd = _e->m_filefd->fd();
#ifndef NDEBUG
    {
        local_lock _lock(m_mutex);
        auto _iter = m_fd_set.find(_fd);
        if (EPOLL_CTL_DEL ==  _opt || EPOLL_CTL_MOD ==  _opt) {
            check(_iter != m_fd_set.end());
            if (EPOLL_CTL_DEL ==  _opt)
                m_fd_set.erase(_fd);
        } else if (EPOLL_CTL_ADD ==  _opt) {
            check(_iter == m_fd_set.end());
            m_fd_set.insert(_fd);
        } else {
            check(!"invalid epoll_ctl option");
        }
    }
#endif

    _e->m_event = {_e->m_flags, (void *)(_e)};
    //log_debug("epoll_ctl: %d, %d, %d", _opt, _fd, _e->m_event.events);
    if (::epoll_ctl(m_epollfd, _opt, _fd, &_e->m_event) < 0) {
        log_error("::epoll_ctl() error - %s:%d", strerror(errno), errno);
        return false;
    }
    return true;
}

__NAMESPACE_END

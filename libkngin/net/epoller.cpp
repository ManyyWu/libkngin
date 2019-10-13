#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include "define.h"
#include "common.h"
#include "timestamp.h"
#include "exception.h"
#include "logfile.h"
#include "epoller.h"
#include "filefd.h"

__NAMESPACE_BEGIN

epoller::epoller (event_loop *_loop)
    try
    :
#ifndef NDEBUG
      m_fd_set(),
#endif
      m_set(RESERVE_EPOLLELR_EVENT),
      m_loop(_loop),
      m_epollfd(::epoll_create1(EPOLL_CLOEXEC))
{
    kassert(_loop);
    if (__fd_invalid(m_epollfd)) {
        log_fatal("::epoll_create1() error - %s:%d", m_epollfd, strerror(errno), errno);
        throw exception("epoller::epoller() error");
    }
} catch (...) {
    if (__fd_valid(m_epollfd))
        ::close(m_epollfd);
    throw;
}

epoller::~epoller ()
{
     this->close();
}

int
epoller::wait (timestamp _ms, epoller::event_list &_list)
{
    int _num = ::epoll_wait(m_epollfd, m_set.data(), (int)m_set.size(), (int)_ms.value_int());
    if (_num > 0) {
        if (_list.size() < m_set.size())
            _list.reserve(_num);
        _list.clear();
        for (auto _iter : m_set) {
            ((epoller_event *)(_iter.data.ptr))->set_flags(_iter.events);
            _list.push_back((epoller_event *)(_iter.data.ptr));
        }
        log_debug("%d events happend", _num);
    } else if (!_num) {
        _list.clear();
        log_debug("no events happend", _num);
    } else {
        log_error("::epoll_wait() error - %s:%d", strerror(errno), errno);
    }
    return std::max(_num, 0);
}

bool
epoller::register_event (epoller_event *_e)
{
    return update_event(EPOLL_CTL_ADD, _e);
}

bool
epoller::remove_event (epoller_event *_e)
{
    return update_event(EPOLL_CTL_DEL, _e);
}

bool
epoller::modify_event (epoller_event *_e)
{
    return update_event(EPOLL_CTL_MOD, _e);
}

void
epoller::close ()
{
#ifndef NDEBUG
    if (!m_fd_set.empty())
        log_warning("there are still have %d undeleted fd in epoller", m_fd_set.size());
#endif
    if (m_epollfd >= 0) {
        if (::close(m_epollfd) < 0)
            log_error("::close() error - %s:%d", strerror(errno), errno);
        m_epollfd = -1;
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
    kassert(_e);

    int _fd = _e->m_filefd->fd();
#ifndef NDEBUG
    auto _iter = m_fd_set.find(_fd);
    if (EPOLL_CTL_DEL ==  _opt || EPOLL_CTL_MOD ==  _opt) {
        kassert(_iter != m_fd_set.end());
        if (EPOLL_CTL_DEL ==  _opt)
            m_fd_set.erase(_fd);
    } else if (EPOLL_CTL_ADD ==  _opt) {
        kassert(_iter == m_fd_set.end());
        m_fd_set.insert(_fd);
    } else {
        kassert(!"invalid epoll_ctl option");
    }
#endif

    epoll_event _epe = {_epe.events = _e->m_flags, _epe.data.ptr = _e};
    if (epoll_ctl(m_epollfd, _opt, _fd, &_epe) < 0) {
        log_error("::epoll_ctl() error - %s:%d", strerror(errno), errno);
        return false;
    }
    return true;
}

__NAMESPACE_END

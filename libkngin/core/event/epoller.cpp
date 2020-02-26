#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_EPOLLER

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"
#include "core/event/epoller.h"

#define KNGIN_FILENAME "libkngin/core/event/epoller.cpp"

KNGIN_NAMESPACE_K_BEGIN

epoller::epoller ()
    try
    : m_events(),
      m_mutex(),
      m_epollfd(::epoll_create1(EPOLL_CLOEXEC))
{
    if (!m_epollfd.valid())
        throw k::system_error("::epoll_create1() error");
} catch (...) {
    log_fatal("epoller::epoller() error");
    throw;
}

epoller::~epoller () KNGIN_NOEXCP
{
    if (m_epollfd.valid())
        ignore_excp(this->close());
}

size_t
epoller::wait (epoll_event_set &_list, timestamp _ms)
{
    assert(_list.size());
    assert(m_epollfd.valid());
    int _num = ::epoll_wait(m_epollfd.fd(), _list.data(),
                            static_cast<int>(_list.size()),
                            static_cast<int>(_ms.value_int()));
    if (_num < 0) {
        if (EINTR == errno)
            return 0;
        throw k::system_error("::epoll_wait() error");
    }

#ifndef NDEBUG
    {
        local_lock _lock(m_mutex);
        for (auto &_iter : m_events)
            if (epoller_event::EVENT_TYPE_TIMER != _iter->type() and
                is_single_ref_ptr(_iter))
                log_warning("an event that does not be cancelled listening, "
                            "and is only managed by epoller");
    }
#endif
    return std::max(_num, 0);
}

void
epoller::close ()
{
    assert(m_epollfd.valid());
    m_epollfd.close();
    m_events.clear();
    if (m_events.size())
        log_warning("there are still have %" PRIu64
                    " undeleted event in epoller", m_events.size());
}

void
epoller::register_event (epoller_event_ptr _e)
{
    assert(_e);
    assert(m_epollfd.valid());
    {
        local_lock _lock(m_mutex);
        assert(!_e->registed());
        m_events.push_back(_e);
        _e->set_registed(true);
        _e->set_index(m_events.back());
        update_event(EPOLL_CTL_ADD, _e->fd(), _e.get());
    }
}

void
epoller::remove_event (epoller_event &_e)
{
    assert(m_epollfd.valid());
    {
        local_lock _lock(m_mutex);
        assert(_e.registed());
        _e.set_registed(false);
        update_event(EPOLL_CTL_DEL, _e.fd(), &_e);
        if (auto _index = _e.index().lock())
            m_events.remove(_index);
    }
}

void
epoller::modify_event (epoller_event &_e)
{
    assert(m_epollfd.valid());
    {
        local_lock _lock(m_mutex);
        assert(_e.registed());
        update_event(EPOLL_CTL_MOD, _e.fd(), &_e);
    }
}

bool
epoller::registed (epoller_event &_e) KNGIN_NOEXCP
{
    assert(m_epollfd.valid());
    {
        //local_lock _lock(m_mutex);
        return _e.registed();
        //return (m_events.find(_e.fd()) != m_events.end());
    }
}

void
epoller::update_event (int _opt, int _fd, epoller_event *_e)
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
     * */
    assert(_e);
    assert(m_epollfd.valid());

    _e->m_event = (epoll_event){_e->m_flags, static_cast<void *>(_e)};
    if (::epoll_ctl(m_epollfd.fd(), _opt, _fd, &_e->m_event) < 0)
        throw k::system_error("::epoll_ctl() error");
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER */

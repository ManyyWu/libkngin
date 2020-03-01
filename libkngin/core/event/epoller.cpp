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
    : events_(),
      mutex_(),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC))
{
    if (!epollfd_.valid())
        throw k::system_error("::epoll_create1() error");
} catch (...) {
    log_fatal("epoller::epoller() error");
    throw;
}

epoller::~epoller () noexcept
{
    if (epollfd_.valid())
        ignore_excp(this->close());
}

size_t
epoller::wait (epoll_event_set &list, timestamp ms)
{
    assert(list.size());
    assert(epollfd_.valid());
    int num = ::epoll_wait(epollfd_.fd(), list.data(),
                            static_cast<int>(list.size()),
                            static_cast<int>(ms.value_int()));
    if (num < 0) {
        if (EINTR == errno)
            return 0;
        throw k::system_error("::epoll_wait() error");
    }

#ifndef NDEBUG
    {
        local_lock lock(mutex_);
        for (auto &iter : events_)
            if (epoller_event::EVENT_TYPE_TIMER != iter->type() and
                is_single_ref_ptr(iter))
                log_warning("an event that does not be cancelled listening, "
                            "and is only managed by epoller");
    }
#endif
    return std::max(num, 0);
}

void
epoller::close ()
{
    assert(epollfd_.valid());
    epollfd_.close();
    events_.clear();
    if (events_.size())
        log_warning("there are still have %" PRIu64
                    " undeleted event in epoller", events_.size());
}

void
epoller::register_event (epoller_event_ptr e)
{
    assert(e);
    assert(epollfd_.valid());
    {
        local_lock lock(mutex_);
        assert(!e->registed());
        events_.push_back(e);
        e->set_registed(true);
        e->set_index(events_.back());
        update_event(EPOLL_CTL_ADD, e->fd(), e.get());
    }
}

void
epoller::remove_event (epoller_event &e)
{
    assert(epollfd_.valid());
    {
        local_lock lock(mutex_);
        assert(e.registed());
        e.set_registed(false);
        update_event(EPOLL_CTL_DEL, e.fd(), &e);
        if (auto index = e.index().lock())
            events_.remove(index);
    }
}

void
epoller::modify_event (epoller_event &e)
{
    assert(epollfd_.valid());
    {
        local_lock lock(mutex_);
        assert(e.registed());
        update_event(EPOLL_CTL_MOD, e.fd(), &e);
    }
}

bool
epoller::registed (epoller_event &e) noexcept
{
    assert(epollfd_.valid());
    {
        //local_lock lock(mutex_);
        return e.registed();
        //return (events_.find(e.fd()) != events_.end());
    }
}

void
epoller::update_event (int opt, int fd, epoller_event *e)
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
    assert(e);
    assert(epollfd_.valid());

    e->event_ = (epoll_event){e->flags_, static_cast<void *>(e)};
    if (::epoll_ctl(epollfd_.fd(), opt, fd, &e->event_) < 0)
        throw k::system_error("::epoll_ctl() error");
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER */

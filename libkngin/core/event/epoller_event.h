#ifndef KNGIN_EPOLLER_EVENT_H
#define KNGIN_EPOLLER_EVENT_H

#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_EPOLLER_EVENT

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <functional>
#include "core/base/filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class epoller;
class event_loop;
class epoller_event : protected filefd {
    friend class epoller;
    friend class event_loop;

public:
    typedef int                            epollfd;

    typedef std::shared_ptr<epoller_event> epoller_event_ptr;

    typedef std::weak_ptr<epoller_event>   epoller_event_weak_ptr ;

    enum EVENT_TYPE {
        EVENT_TYPE_TIMER = 0,
        EVENT_TYPE_EVENT = 1,
        EVENT_TYPE_FILE  = 2
    };

public:
    epoller_event  () = delete;

    explicit
    epoller_event  (epollfd fd, EVENT_TYPE type,
                    uint8_t priority = UINT8_MAX) noexcept;

    epoller_event  (epoller_event &&e) noexcept;

    virtual
    ~epoller_event () noexcept {};

public:
    bool
    registed       () const noexcept
    { return registed_; }

    EVENT_TYPE
    type           () const noexcept
    { return type_; }

    uint8_t
    priority       () const noexcept
    { return priority_; }

protected:
    void
    set_registed   (bool on) noexcept
    { registed_ = on; }

    void
    set_index      (epoller_event_ptr &e)
    { index_ = e; }

    epoller_event_weak_ptr &
    index          () noexcept
    { return index_; }

protected:
    void
    set_flags      (uint32_t flags) noexcept { flags_ = flags; }
    uint32_t
    flags          () const          noexcept { return flags_; }
    void
    enable_read    ()                noexcept { flags_ |= EPOLLIN; }
    void
    enable_write   ()                noexcept { flags_ |= EPOLLOUT; }
    void
    enable_oob     ()                noexcept { flags_ |= EPOLLPRI; }
    void
    enable_once    ()                noexcept { flags_ |= EPOLLONESHOT; }
    void
    enable_et      ()                noexcept { flags_ |= EPOLLET; }
    void
    disable_read   ()                noexcept { flags_ &= ~EPOLLIN; }
    void
    disable_write  ()                noexcept { flags_ &= ~EPOLLOUT; }
    void
    disable_oob    ()                noexcept { flags_ &= ~EPOLLPRI; }
    void
    disable_once   ()                noexcept { flags_ &= ~EPOLLONESHOT; }
    void
    disable_et     ()                noexcept { flags_ &= ~EPOLLET; }
    void
    disable_all    ()                noexcept { flags_ = EPOLLHUP | EPOLLERR; }
    bool
    pollin         () const          noexcept { return (flags_ & EPOLLIN); }
    bool
    pollout        () const          noexcept { return (flags_ & EPOLLOUT); }
    bool
    pollpri        () const          noexcept { return (flags_ & EPOLLPRI); }
    bool
    pollonce       () const          noexcept { return (flags_ & EPOLLONESHOT); }
    bool
    et             () const          noexcept { return (flags_ & EPOLLET); }

protected:
    virtual void
    on_events      (event_loop &loop, uint32_t flags) = 0;

private:
    uint32_t               flags_;

    epoll_event            event_;

    bool                   registed_;

    EVENT_TYPE             type_;

    uint8_t                priority_;

    epoller_event_weak_ptr index_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EPOLLER_EVENT_H */

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */

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
    epoller_event  (epollfd _fd, EVENT_TYPE _type, 
                    uint8_t _priority = UINT8_MAX) noexcept;

    epoller_event  (epoller_event &&_e) noexcept;

    virtual
    ~epoller_event () noexcept {};

public:
    bool
    registed       () const noexcept
    { return m_registed; }

    EVENT_TYPE
    type           () const noexcept
    { return m_type; }

    uint8_t
    priority       () const noexcept
    { return m_priority; }

protected:
    void
    set_registed   (bool _on) noexcept
    { m_registed = _on; }

    void
    set_index      (epoller_event_ptr &_e)
    { m_index = _e; }

    epoller_event_weak_ptr &
    index          () noexcept
    { return m_index; }

protected:
    void
    set_flags      (uint32_t _flags) noexcept { m_flags = _flags; }
    uint32_t
    flags          () const          noexcept { return m_flags; }
    void
    enable_read    ()                noexcept { m_flags |= EPOLLIN; }
    void
    enable_write   ()                noexcept { m_flags |= EPOLLOUT; }
    void
    enable_oob     ()                noexcept { m_flags |= EPOLLPRI; }
    void
    enable_once    ()                noexcept { m_flags |= EPOLLONESHOT; }
    void
    enable_et      ()                noexcept { m_flags |= EPOLLET; }
    void
    disable_read   ()                noexcept { m_flags &= ~EPOLLIN; }
    void
    disable_write  ()                noexcept { m_flags &= ~EPOLLOUT; }
    void
    disable_oob    ()                noexcept { m_flags &= ~EPOLLPRI; }
    void
    disable_once   ()                noexcept { m_flags &= ~EPOLLONESHOT; }
    void
    disable_et     ()                noexcept { m_flags &= ~EPOLLET; }
    void
    disable_all    ()                noexcept { m_flags = EPOLLHUP | EPOLLERR; }
    bool
    pollin         () const          noexcept { return (m_flags & EPOLLIN); }
    bool
    pollout        () const          noexcept { return (m_flags & EPOLLOUT); }
    bool
    pollpri        () const          noexcept { return (m_flags & EPOLLPRI); }
    bool
    pollonce       () const          noexcept { return (m_flags & EPOLLONESHOT); }
    bool
    et             () const          noexcept { return (m_flags & EPOLLET); }

protected:
    virtual void
    on_events      (event_loop &_loop, uint32_t _flags) = 0;

private:
    uint32_t               m_flags;

    epoll_event            m_event;

    bool                   m_registed;

    EVENT_TYPE             m_type;

    uint8_t                m_priority;

    epoller_event_weak_ptr m_index;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EPOLLER_EVENT_H */

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */

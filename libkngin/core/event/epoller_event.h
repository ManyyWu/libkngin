#ifndef KNGIN_EPOLLER_EVENT_H
#define KNGIN_EPOLLER_EVENT_H

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <functional>
#include <system_error>
#include "core/base/define.h"
#include "core/base/filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class epoller;
class event_loop;
class epoller_event : protected filefd {
    friend class epoller;
    friend class event_loop;

public:
    typedef int     epollfd;

    enum EVENT_TYPE {
        EVENT_TYPE_TIMER = 0,
        EVENT_TYPE_EVENT = 1,
        EVENT_TYPE_FILE  = 2
    };

public:
    epoller_event  () = delete;

    explicit
    epoller_event  (epollfd _fd, EVENT_TYPE _type, 
                    uint8_t _priority = UINT8_MAX) KNGIN_NOEXCP;

    epoller_event  (epoller_event &&_e) KNGIN_NOEXCP;

    virtual
    ~epoller_event () KNGIN_NOEXCP {};

public:
    bool
    registed       () const KNGIN_NOEXCP
    { return m_registed; }

    EVENT_TYPE
    type           () const KNGIN_NOEXCP
    { return m_type; }

    uint8_t
    priority       () const KNGIN_NOEXCP
    { return m_priority; }

protected:
    void
    set_flags      (uint32_t _flags) KNGIN_NOEXCP { m_flags = _flags; }
    uint32_t
    flags          () const          KNGIN_NOEXCP { return m_flags; }
    void
    enable_read    ()                KNGIN_NOEXCP { m_flags |= EPOLLIN; }
    void
    enable_write   ()                KNGIN_NOEXCP { m_flags |= EPOLLOUT; }
    void
    enable_oob     ()                KNGIN_NOEXCP { m_flags |= EPOLLPRI; }
    void
    enable_once    ()                KNGIN_NOEXCP { m_flags |= EPOLLONESHOT; }
    void
    enable_et      ()                KNGIN_NOEXCP { m_flags |= EPOLLET; }
    void
    disable_read   ()                KNGIN_NOEXCP { m_flags &= ~EPOLLIN; }
    void
    disable_write  ()                KNGIN_NOEXCP { m_flags &= ~EPOLLOUT; }
    void
    disable_oob    ()                KNGIN_NOEXCP { m_flags &= ~EPOLLPRI; }
    void
    disable_once   ()                KNGIN_NOEXCP { m_flags &= ~EPOLLONESHOT; }
    void
    disable_et     ()                KNGIN_NOEXCP { m_flags &= ~EPOLLET; }
    void
    disable_all    ()                KNGIN_NOEXCP { m_flags = EPOLLHUP | EPOLLERR; }
    bool
    pollin         () const          KNGIN_NOEXCP { return (m_flags & EPOLLIN); }
    bool
    pollout        () const          KNGIN_NOEXCP { return (m_flags & EPOLLOUT); }
    bool
    pollpri        () const          KNGIN_NOEXCP { return (m_flags & EPOLLPRI); }
    bool
    pollonce       () const          KNGIN_NOEXCP { return (m_flags & EPOLLONESHOT); }
    bool
    et             () const          KNGIN_NOEXCP { return (m_flags & EPOLLET); }

protected:
    virtual void
    on_events      (event_loop &_loop, uint32_t _flags) = 0;

private:
    uint32_t    m_flags;

    epoll_event m_event;

    bool        m_registed;

    EVENT_TYPE  m_type;

    uint8_t     m_priority;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EPOLLER_EVENT_H */

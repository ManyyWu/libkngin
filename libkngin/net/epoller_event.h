#ifndef _EPOLLER_EVENT_H_
#define _EPOLLER_EVENT_H_

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <functional>
#include <system_error>
#include "core/define.h"
#include "core/noncopyable.h"
#include "net/filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class epoller;
class event_loop;
class event_loop_pimpl;
class epoller_event : public filefd {
    typedef int epollfd;

public:
    epoller_event  () = delete;

    epoller_event  (epollfd _fd) KNGIN_NOEXP;

    explicit
    epoller_event  (epoller_event &&_e) KNGIN_NOEXP;

    virtual
    ~epoller_event () KNGIN_NOEXP {};

public:
    void
    set_flags      (uint32_t _flags) KNGIN_NOEXP { m_flags = _flags; }
    uint32_t
    flags          () const          KNGIN_NOEXP { return m_flags; }
    void
    enable_read    ()                KNGIN_NOEXP { m_flags |= EPOLLIN; }
    void
    enable_write   ()                KNGIN_NOEXP { m_flags |= EPOLLOUT; }
    void
    enable_oob     ()                KNGIN_NOEXP { m_flags |= EPOLLPRI; }
    void
    enable_once    ()                KNGIN_NOEXP { m_flags |= EPOLLONESHOT; }
    void
    disable_read   ()                KNGIN_NOEXP { m_flags &= ~EPOLLIN; }
    void
    disable_write  ()                KNGIN_NOEXP { m_flags &= ~EPOLLOUT; }
    void
    disable_oob    ()                KNGIN_NOEXP { m_flags &= ~EPOLLPRI; }
    void
    disable_once   ()                KNGIN_NOEXP { m_flags &= ~EPOLLONESHOT; }
    void
    disable_all    ()                KNGIN_NOEXP { m_flags = EPOLLHUP | EPOLLERR; }
    bool
    pollin         () const          KNGIN_NOEXP { return (m_flags & EPOLLIN); }
    bool
    pollout        () const          KNGIN_NOEXP { return (m_flags & EPOLLOUT); }
    bool
    pollpri        () const          KNGIN_NOEXP { return (m_flags & EPOLLPRI); }
    bool
    pollonce       () const          KNGIN_NOEXP { return (m_flags & EPOLLONESHOT); }

private:
    void
    on_events      (uint32_t _events);

    virtual void
    on_error       () = 0;

    virtual void
    on_read        () = 0;

    virtual void
    on_write       () = 0;

    virtual void
    on_oob         () = 0;

private:
    uint32_t              m_flags;

    epoll_event           m_event;

private:
    friend class epoller;

    friend class event_loop_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _EPOLLER_EVENT_H_ */

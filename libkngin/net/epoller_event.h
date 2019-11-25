#ifndef _EPOLLER_EVENT_H_
#define _EPOLLER_EVENT_H_

#include <functional>
#include <sys/epoll.h>
#include "core/define.h"
#include "net/filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class epoller_event {
public:
    typedef std::function<void (void)> epoller_event_cb;

    typedef int                        epollfd;

public:
    epoller_event  () = delete;

    epoller_event  (event_loop *_loop, filefd *_s);

    ~epoller_event ();

public:
    void
    set_flags      (int flags) { m_flags = flags; }
    int
    flags          () const    { return m_flags; }
    void
    enable_read    ()          { m_flags |= EPOLLIN; }
    void
    enable_write   ()          { m_flags |= EPOLLOUT; }
    void
    enable_oob     ()          { m_flags |= EPOLLPRI; }
    void
    enable_once    ()          { m_flags |= EPOLLONESHOT; }
    void
    disable_read   ()          { m_flags &= ~EPOLLIN; }
    void
    disable_write  ()          { m_flags &= ~EPOLLOUT; }
    void
    disable_oob    ()          { m_flags &= ~EPOLLPRI; }
    void
    disable_once   ()          { m_flags &= ~EPOLLONESHOT; }
    void
    disable_close  ()          { m_closecb = nullptr; }
    void
    disable_all    ()          { m_flags = EPOLLHUP | EPOLLERR; }
    bool
    pollin         () const    { return (m_flags & EPOLLIN); }
    bool
    pollout        () const    { return (m_flags & EPOLLOUT); }
    bool
    pollpri        () const    { return (m_flags & EPOLLPRI); }
    bool
    pollonce       () const    { return (m_flags & EPOLLONESHOT); }
    bool
    registed       () const    { return m_registed; }

public:
    void
    start          ();

    void
    update         ();

    void
    stop           ();

    void
    remove         ();

public:
    void
    set_read_cb    (epoller_event_cb &&_fn) { m_incb = std::move(_fn); m_flags |= EPOLLIN; }
    void
    set_write_cb   (epoller_event_cb &&_fn) { m_outcb = std::move(_fn); m_flags |= EPOLLOUT; }
    void
    set_error_cb   (epoller_event_cb &&_fn) { m_errcb = std::move(_fn); m_flags |= EPOLLERR; }
    void
    set_oob_cb     (epoller_event_cb &&_fn) { m_pricb = std::move(_fn); m_flags |= EPOLLPRI; }
    void
    set_close_cb   (epoller_event_cb &&_fn) { m_closecb = std::move(_fn); m_flags |= EPOLLHUP; }

protected:
    void
    on_events  (uint32_t _flags);

protected:
    event_loop *      m_loop;

    filefd *          m_filefd;

    uint32_t          m_flags;

    epoller_event_cb  m_incb;

    epoller_event_cb  m_outcb;

    epoller_event_cb  m_errcb;

    epoller_event_cb  m_pricb;

    epoller_event_cb  m_closecb;

    epoll_event       m_event;

    std::atomic<bool> m_registed;

protected:
    friend class epoller;
    friend class event_loop;
};

KNGIN_NAMESPACE_K_END

#endif /* _EPOLLER_EVENT_H_ */

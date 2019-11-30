#ifndef _EPOLLER_EVENT_H_
#define _EPOLLER_EVENT_H_

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <functional>
#include <memory>
#include "core/define.h"
#include "core/noncopyable.h"
#include "net/filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class epoller;
class event_loop;
class event_loop_pimpl;
class epoller_event : noncopyable {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef std::function<void (void)>        epoller_event_cb;

    typedef int                               epollfd;

public:
    epoller_event  () = delete;

    epoller_event  (event_loop_pimpl_ptr _loop, filefd *_s) KNGIN_EXP;

    ~epoller_event () KNGIN_NOEXP;

public:
    void
    set_flags      (int flags) KNGIN_NOEXP { m_flags = flags; }
    int
    flags          () const    KNGIN_NOEXP { return m_flags; }
    void
    enable_read    ()          KNGIN_NOEXP { m_flags |= EPOLLIN; }
    void
    enable_write   ()          KNGIN_NOEXP { m_flags |= EPOLLOUT; }
    void
    enable_oob     ()          KNGIN_NOEXP { m_flags |= EPOLLPRI; }
    void
    enable_once    ()          KNGIN_NOEXP { m_flags |= EPOLLONESHOT; }
    void
    disable_read   ()          KNGIN_NOEXP { m_flags &= ~EPOLLIN; }
    void
    disable_write  ()          KNGIN_NOEXP { m_flags &= ~EPOLLOUT; }
    void
    disable_oob    ()          KNGIN_NOEXP { m_flags &= ~EPOLLPRI; }
    void
    disable_once   ()          KNGIN_NOEXP { m_flags &= ~EPOLLONESHOT; }
    void
    disable_close  ()          KNGIN_NOEXP { m_closecb = nullptr; }
    void
    disable_all    ()          KNGIN_NOEXP { m_flags = EPOLLHUP | EPOLLERR; }
    bool
    pollin         () const    KNGIN_NOEXP { return (m_flags & EPOLLIN); }
    bool
    pollout        () const    KNGIN_NOEXP { return (m_flags & EPOLLOUT); }
    bool
    pollpri        () const    KNGIN_NOEXP { return (m_flags & EPOLLPRI); }
    bool
    pollonce       () const    KNGIN_NOEXP { return (m_flags & EPOLLONESHOT); }
    bool
    registed       () const    KNGIN_NOEXP { return m_registed; }

public:
    void
    start          () KNGIN_EXP;

    void
    update         () KNGIN_EXP;

    void
    stop           () KNGIN_EXP;

    void
    remove         () KNGIN_EXP;

public:
    void
    set_read_cb    (epoller_event_cb &&_fn) KNGIN_NOEXP
    { m_incb = std::move(_fn); m_flags |= EPOLLIN; }
    void
    set_write_cb   (epoller_event_cb &&_fn) KNGIN_NOEXP
    { m_outcb = std::move(_fn); m_flags |= EPOLLOUT; }
    void
    set_error_cb   (epoller_event_cb &&_fn) KNGIN_NOEXP
    { m_errcb = std::move(_fn); m_flags |= EPOLLERR; }
    void
    set_oob_cb     (epoller_event_cb &&_fn) KNGIN_NOEXP
    { m_pricb = std::move(_fn); m_flags |= EPOLLPRI; }
    void
    set_close_cb   (epoller_event_cb &&_fn) KNGIN_NOEXP
    { m_closecb = std::move(_fn); m_flags |= EPOLLHUP; }

protected:
    void
    on_events      (uint32_t _flags) KNGIN_EXP;

protected:
    event_loop_pimpl_ptr m_loop;

    filefd *             m_filefd;

    uint32_t             m_flags;

    epoller_event_cb     m_incb;

    epoller_event_cb     m_outcb;

    epoller_event_cb     m_errcb;

    epoller_event_cb     m_pricb;

    epoller_event_cb     m_closecb;

    epoll_event          m_event;

    std::atomic<bool>    m_registed;

private:
    friend class epoller;

    friend class event_loop_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _EPOLLER_EVENT_H_ */

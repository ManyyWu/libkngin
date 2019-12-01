#ifndef _EPOLLER_EVENT_H_
#define _EPOLLER_EVENT_H_

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <functional>
#include <memory>
#include <atomic>
#include "core/define.h"
#include "core/noncopyable.h"
#include "net/filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class epoller;
class event_loop;
class event_loop_pimpl;
class epoller_event : public noncopyable {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef std::function<void (void)>        epoller_event_handler;

    typedef int                               epollfd;

public:
    epoller_event  () = delete;

    epoller_event  (event_loop_pimpl_ptr _loop, filefd *_s);

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
    disable_close  ()          KNGIN_NOEXP { m_close_handler = nullptr; }
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
    start          ();

    void
    update         ();

    void
    stop           ();

    void
    remove         ();

public:
    void
    set_read_handler  (epoller_event_handler &&_fn) KNGIN_NOEXP
    { m_in_handler = std::move(_fn); m_flags |= EPOLLIN; }
    void
    set_write_handler (epoller_event_handler &&_fn) KNGIN_NOEXP
    { m_out_handler = std::move(_fn); m_flags |= EPOLLOUT; }
    void
    set_error_handler (epoller_event_handler &&_fn) KNGIN_NOEXP
    { m_err_handler = std::move(_fn); m_flags |= EPOLLERR; }
    void
    set_oob_handler   (epoller_event_handler &&_fn) KNGIN_NOEXP
    { m_pri_handler = std::move(_fn); m_flags |= EPOLLPRI; }
    void
    set_close_handler (epoller_event_handler &&_fn) KNGIN_NOEXP
    { m_close_handler = std::move(_fn); m_flags |= EPOLLHUP; }

private:
    void
    on_events      (uint32_t _flags);

private:
    event_loop_pimpl_ptr  m_loop;

    filefd *              m_filefd;

    uint32_t              m_flags;

    epoller_event_handler m_in_handler;

    epoller_event_handler m_out_handler;

    epoller_event_handler m_err_handler;

    epoller_event_handler m_pri_handler;

    epoller_event_handler m_close_handler;

    epoll_event           m_event;

    std::atomic<bool>     m_registed;

private:
    friend class epoller;

    friend class event_loop_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _EPOLLER_EVENT_H_ */

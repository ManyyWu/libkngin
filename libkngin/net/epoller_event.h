#ifndef _EPOLLER_EVENT_H_
#define _EPOLLER_EVENT_H_

#include <functional>
#include <sys/epoll.h>
#include "define.h"
#include "noncopyable.h"
#include "filefd.h"

__NAMESPACE_BEGIN

class event_loop;
class epoller_event : noncopyable {
public:
    typedef std::function<void (filefd *)> epoller_event_cb;

    typedef int                            epollfd;

public:
    epoller_event  () = delete;

    explicit
    epoller_event  (event_loop *_loop, filefd *_s);

    ~epoller_event () = default;

public:
    void
    set_flags      (int flags) { m_flags = flags; }
    int
    flags          () const    { return m_flags; }
    void
    enable_once    ()          { m_flags = m_flags & ~EPOLLONESHOT; }
    void
    disable_read   ()          { m_flags = m_flags & ~EPOLLIN; m_incb = nullptr; }
    void
    disable_write  ()          { m_flags = m_flags & ~EPOLLOUT; m_outcb = nullptr; }
    void
    disable_error  ()          { m_errcb = nullptr; }
    void
    disable_ergent ()          { m_flags = m_flags & ~EPOLLPRI; m_pricb = nullptr; }
    void
    disable_once   ()          { m_flags = m_flags & ~EPOLLONESHOT; }
    void
    disable_close  ()          { m_closecb = nullptr; }
    bool
    pollin         () const    { return (m_flags & EPOLLIN); }
    bool
    pollout        () const    { return (m_flags & EPOLLOUT); }
    bool
    pollpri        () const    { return (m_flags & EPOLLPRI); }
    bool
    pollonce       () const    { return (m_flags & EPOLLONESHOT); }
    bool
    pollhup        () const    { return (m_flags & EPOLLHUP); }

    void
    update         ();

public:
    void
    set_read_cb    (epoller_event_cb &&_fn) { m_incb = std::move(_fn); m_flags |= EPOLLIN; }
    void
    set_write_cb   (epoller_event_cb &&_fn) { m_outcb = std::move(_fn); m_flags |= EPOLLOUT; }
    void
    set_error_cb   (epoller_event_cb &&_fn) { m_errcb = std::move(_fn); m_flags |= EPOLLERR; }
    void
    set_ergent_cb  (epoller_event_cb &&_fn) { m_pricb = std::move(_fn); m_flags |= EPOLLPRI; }
    void
    set_close_cb   (epoller_event_cb &&_fn) { m_closecb = std::move(_fn); m_flags |= EPOLLHUP; }

public:
    void
    handle_events  ();

protected:
    event_loop *     m_loop;

    filefd *         m_filefd;

    int              m_flags;

    epoller_event_cb m_incb;

    epoller_event_cb m_outcb;

    epoller_event_cb m_errcb;

    epoller_event_cb m_pricb;

    epoller_event_cb m_closecb;

protected:
    friend class epoller;
};

__NAMESPACE_END

#endif /* _EPOLLER_EVENT_H_ */

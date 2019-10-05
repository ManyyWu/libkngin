#ifndef _epoller_event_H_
#define _epoller_event_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "socket.h"

__NAMESPACE_BEGIN

typedef int epollfd;

class epoller;
class event_loop;
class epoller_event : noncopyable {
public:
    typedef std::function<void (socket *)> epoller_event_cb;

public:
    epoller_event  (event_loop *_loop, socket *_s);

    ~epoller_event ();

public:
    void
    set_flags      (int flags);

    void
    set_pollin     (bool _en);

    void
    set_pollout    (bool _en);

    void
    set_pollerr    (bool _en);

    void
    set_pollpri    (bool _en);

    void
    set_pollhup    (bool _en);

    void
    set_pollonce   (bool _en);

    bool
    pollin         () const;

    bool
    pollout        () const;

    bool
    pollerr        () const;

    bool
    pollpri        () const;

    bool
    pollhup        () const;

    bool
    pollonce       () const;

    void
    update         ();

public:
    void
    set_read_cb    (epoller_event_cb _fn);

    void
    set_write_cb   (epoller_event_cb _fn);

    void
    set_error_cb   (epoller_event_cb _fn);

    void
    set_ergent_cb  (epoller_event_cb _fn);

    void
    set_close_cb   (epoller_event_cb _fn);

public:
    void
    handle_events  ();

protected:
    event_loop *     m_loop;

    socket *         m_socket;

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

#endif /* _epoller_event_H_ */

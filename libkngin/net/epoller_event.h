#ifndef _EPOLLER_EVENT_H_
#define _EPOLLER_EVENT_H_

#include <functional>
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
    epoller_event  (event_loop *_loop, filefd *_s);

    ~epoller_event   ();

public:
    void
    set_flags        (int flags);

    int
    flags            () const;

    void
    enable_once      (); // for EPOLLONCE

    void
    disable_read     (); // for EPOLLIN

    void
    disable_write    (); // for EPOLLOUT

    void
    disable_error    (); // for EPOLLERR

    void
    disable_ergent   (); // for EPOLLPRI

    void
    disable_once     (); // for EPOLLONCE

    void
    disable_close    (); // for EPOLLHUP

    bool
    pollin           () const;

    bool
    pollout          () const;

    bool
    pollpri          () const;

    bool
    pollonce         () const;

    bool
    pollhup          () const;

    void
    update           ();

public:
    void
    set_read_cb      (epoller_event_cb _fn); // for EPOLLIN

    void
    set_write_cb     (epoller_event_cb _fn); // for EPOLLOUT

    void
    set_error_cb     (epoller_event_cb _fn); // for EPOLLPRI

    void
    set_ergent_cb    (epoller_event_cb _fn); // for EPOLLONCE

    void
    set_close_cb     (epoller_event_cb _fn); // for EPOLLHUP

public:
    void
    handle_events    ();

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

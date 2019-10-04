#ifndef _EPOLL_EVENT_H_
#define _EPOLL_EVENT_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "socket.h"

__NAMESPACE_BEGIN

typedef int epollfd;

class epoll;
class event_loop;
class epoll_event : noncopyable {
public:
    typedef std::function<void (socket &)> epoll_event_cb;

public:
    epoll_event  (event_loop *_loop, socket *_s);

    ~epoll_event ();

public:
    void
    set_pollin  (bool _en);

    void
    set_pollout (bool _en);

    void
    set_pollerr (bool _en);

    void
    set_pollpri (bool _en);

    void
    set_pollhup (bool _en);

    bool
    pollin      ();

    bool
    pollout     ();

    bool
    pollerr     ();

    bool
    pollpri     ();

    bool
    pollhup     ();

protected:
    epoll_event_cb m_cb;

    socket         m_socket;

    int            m_flags;

    uint32_t       m_index;

protected:
    friend class epoll;
};

__NAMESPACE_END

#endif /* _EPOLL_EVENT_H_ */

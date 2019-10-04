#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>
#include <vector>
#include "define.h"
#include "noncopyable.h"
#include "event_loop.h"
#include "epoll_event.h"

#define RESERVE_EPOLL_EVENT 32

__NAMESPACE_BEGIN

class epoll : public noncopyable {
public:
    class __event {}; // 1byte

public:
    typedef std::vector<__event> event_set;

public:
    epoll          (event_loop *_loop);

    ~epoll         ();

public:
    uint32_t
    register_event (epoll_event *_e);

    uint32_t
    update_event   (epoll_event *_e);

    uint32_t
    remove_event   (epoll_event *_e);

    epoll_event &
    find           (int _fd);

public:
    void
    set_et         () = delete;

    void
    set_lt         () = delete;

public:
    void
    close          ();

protected:
    event_set   m_events;

    event_loop *m_loop;

    int         m_epollfd;
};

__NAMESPACE_END

#endif /* _EPOLL_H_ */
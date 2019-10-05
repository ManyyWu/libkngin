#ifndef _EPOLL_H_
#define _EPOLL_H_

#ifndef NDEBUG
#include <set>
#endif
#include <sys/epoll.h>
#include <vector>
#include "define.h"
#include "noncopyable.h"
#include "timestamp.h"
#include "event_loop.h"
#include "epoller_event.h"

#define RESERVE_EPOLLELR_EVENT 32

__NAMESPACE_BEGIN

class epoller : public noncopyable {
public:
    typedef std::vector<struct epoll_event> __epoll_event_set;

    typedef std::vector<epoller_event *>    event_list;

public:
    epoller        (event_loop *_loop);

    ~epoller       ();

public:
    int
    wait           (timestamp _ms, epoller::event_list &_list);

public:
    bool
    register_event (epoller_event *_e);

    bool
    remove_event   (epoller_event *_e);

    bool
    modify_event   (epoller_event *_e);

public:
    void
    set_et         () = delete;

    void
    set_lt         () = delete;

public:
    void
    close          ();

protected:
    bool
    update_event   (int _opt, epoller_event *_e);

protected:
#ifndef NDEBUG
    std::set<int>     m_fd_set;
#endif

    __epoll_event_set m_set;

    event_loop *      m_loop;

    int               m_epollfd;
};

__NAMESPACE_END

#endif /* _EPOLL_H_ */
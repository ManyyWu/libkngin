#ifndef _EPOLL_H_
#define _EPOLL_H_

#ifndef NDEBUG
#include <set>
#endif
#include <sys/epoll.h>
#include <vector>
#include "define.h"
#include "timestamp.h"
#include "epoller_event.h"

#define RESERVED_EPOLLELR_EVENT 32

__NAMESPACE_BEGIN

class event_loop;
class epoller {
public:
    typedef std::vector<struct epoll_event> epoll_event_set;

public:
    epoller        () = delete;

    explicit
    epoller        (event_loop *_loop);

    ~epoller       ();

public:
    uint32_t
    wait           (epoller::epoll_event_set &_list, timestamp _ms);

public:
    void
    set_et         () = delete;

    void
    set_lt         () = delete;

public:
    void
    close          ();

    bool
    closed         () { return __fd_invalid(m_epollfd); }

protected:
    bool
    register_event (epoller_event *_e) { return update_event(EPOLL_CTL_ADD, _e); }

    bool
    remove_event   (epoller_event *_e) { return update_event(EPOLL_CTL_DEL, _e); }

    bool
    modify_event   (epoller_event *_e) { return update_event(EPOLL_CTL_MOD, _e); }

    bool
    update_event   (int _opt, epoller_event *_e);

protected:
#ifndef NDEBUG
    std::set<int> m_fd_set;
#endif

    event_loop *  m_loop;

    int           m_epollfd;

protected:
    friend class epoller_event;
    friend class event_loop;
};

__NAMESPACE_END

#endif /* _EPOLL_H_ */
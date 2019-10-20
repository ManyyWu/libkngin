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
#include "epoller_event.h"

#define RESERVE_EPOLLELR_EVENT 32

__NAMESPACE_BEGIN

class event_loop;
class epoller : public noncopyable {
public:
    typedef std::vector<struct epoll_event> epoll_event_set;

    typedef std::vector<epoller_event *>    event_list;

public:
    explicit
    epoller        (event_loop *_loop);

    ~epoller       ();

public:
    int
    wait           (timestamp _ms, epoller::event_list &_list);

public:
    bool
    register_event (epoller_event *_e) { return update_event(EPOLL_CTL_ADD, _e); }

    bool
    remove_event   (epoller_event *_e) { return update_event(EPOLL_CTL_DEL, _e); }

    bool
    modify_event   (epoller_event *_e) { return update_event(EPOLL_CTL_MOD, _e); }

public:
    void
    set_et         () = delete;

    void
    set_lt         () = delete;

public:
    void
    close          ();

    bool
    update_event   (int _opt, epoller_event *_e);

protected:
#ifndef NDEBUG
    std::set<int>   m_fd_set;
#endif

    epoll_event_set m_set;

    event_loop *    m_loop;

    int             m_epollfd;
};

__NAMESPACE_END

#endif /* _EPOLL_H_ */
#ifndef _EPOLL_H_
#define _EPOLL_H_

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#ifndef NDEBUG
#include <set>
#endif
#include <memory>
#include <vector>
#include "core/define.h"
#include "core/mutex.h"
#include "core/timestamp.h"
#include "net/epoller_event.h"

#define RESERVED_EPOLLELR_EVENT 32

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event_loop_pimpl;
class epoller : noncopyable {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef std::vector<struct epoll_event>   epoll_event_set;

public:
    epoller        () = delete;

    explicit
    epoller        (event_loop &_loop);

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
    closed         () { return !m_epollfd.valid(); }

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
    std::set<int>        m_fd_set;

    mutex                m_mutex;
#endif

    event_loop_pimpl_ptr m_loop_pimpl;

    filefd               m_epollfd;

protected:
    friend class epoller_event;

    friend class event_loop;
};

KNGIN_NAMESPACE_K_END

#endif /* _EPOLL_H_ */
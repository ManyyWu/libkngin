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
    epoller        () KNGIN_NOEXP = delete;

    explicit
    epoller        (event_loop &_loop) KNGIN_EXP;

    ~epoller       () KNGIN_NOEXP;

public:
    uint32_t
    wait           (epoller::epoll_event_set &_list, timestamp _ms) KNGIN_EXP;

public:
    void
    set_et         () = delete;

    void
    set_lt         () = delete;

public:
    void
    close          () KNGIN_EXP;

    bool
    closed         () KNGIN_NOEXP
    { return !m_epollfd.valid(); }

protected:
    void
    register_event (epoller_event *_e) KNGIN_EXP
    { update_event(EPOLL_CTL_ADD, _e); }

    void
    remove_event   (epoller_event *_e) KNGIN_EXP
    { update_event(EPOLL_CTL_DEL, _e); }

    void
    modify_event   (epoller_event *_e) KNGIN_EXP
    { update_event(EPOLL_CTL_MOD, _e); }

    void
    update_event   (int _opt, epoller_event *_e) KNGIN_EXP;

protected:
#ifndef NDEBUG
    std::set<int>        m_fd_set;

    mutex                m_mutex;
#endif

    event_loop_pimpl_ptr m_loop_pimpl;

    filefd               m_epollfd;
};

KNGIN_NAMESPACE_K_END

#endif /* _EPOLL_H_ */
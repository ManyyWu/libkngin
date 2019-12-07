#ifndef _EPOLL_H_
#define _EPOLL_H_

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <memory>
#include <vector>
#include <unordered_map>
#include "core/define.h"
#include "core/mutex.h"
#include "core/timestamp.h"
#include "net/epoller_event.h"

#define RESERVED_EPOLLELR_EVENT 32

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event_loop_pimpl;
class epoller : public noncopyable {
public:
    typedef std::shared_ptr<event_loop_pimpl>          event_loop_pimpl_ptr;

    typedef std::shared_ptr<epoller_event>             epoller_event_ptr;

    typedef std::unordered_map<int, epoller_event_ptr> epoller_event_map;

    typedef std::vector<struct epoll_event>            epoll_event_set;

public:
    epoller        () = delete;

    explicit
    epoller        (event_loop_pimpl_ptr _loop);

    ~epoller       () KNGIN_NOEXP;

public:
    uint32_t
    wait           (epoll_event_set &_list, timestamp _ms);

public:
    void
    set_et         () = delete;

    void
    set_lt         () = delete;

public:
    void
    close          ();

    bool
    closed         () KNGIN_NOEXP
    { return !m_epollfd.valid(); }

public:
    void
    register_event (epoller_event_ptr _e);

    void
    remove_event   (epoller_event_ptr &_e);

    void
    modify_event   (epoller_event_ptr &_e);

    void
    update_event   (int _opt, int _fd, epoller_event *_e);

protected:
    event_loop_pimpl_ptr m_loop_pimpl;

    epoller_event_map    m_events;

    mutex                m_mutex;

    filefd               m_epollfd;
};

KNGIN_NAMESPACE_K_END

#endif /* _EPOLL_H_ */
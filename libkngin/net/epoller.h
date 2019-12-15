#ifndef KNGIN_EPOLL_H
#define KNGIN_EPOLL_H

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

class epoller : public noncopyable {
public:
    typedef std::vector<struct ::epoll_event>          epoll_event_set;

    typedef std::shared_ptr<epoller_event>             epoller_event_ptr;

    typedef std::unordered_map<int, epoller_event_ptr> epoller_event_map;

public:
    epoller        ();

    ~epoller       () KNGIN_NOEXCP;

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
    closed         () KNGIN_NOEXCP
    { return !m_epollfd.valid(); }

public:
    bool
    registed       (int _fd) KNGIN_NOEXCP;

    void
    register_event (epoller_event_ptr _e);

    void
    remove_event   (epoller_event_ptr _e);

    void
    modify_event   (epoller_event_ptr _e);

private:
    void
    update_event   (int _opt, int _fd, epoller_event *_e);

private:
    epoller_event_map    m_events;

    mutex                m_mutex;

    filefd               m_epollfd;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EPOLL_H */
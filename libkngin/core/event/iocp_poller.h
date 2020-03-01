#ifndef KNGIN_IOCP_POLLER_H
#define KNGIN_IOCP_POLLER_H

#include "core/base/define.h"
#ifndef KNGIN_FLAG_HAVE_EPOLLER

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <memory>
#include <vector>
#include <list>
#include "core/base/mutex.h"
#include "core/base/timestamp.h"
#include "core/event/iocp_event.h"

KNGIN_NAMESPACE_K_BEGIN

class iocp_poller: public noncopyable {
public:
    typedef iocp_event::iocp_event_ptr      iocp_event_ptr;

    typedef iocp_event::iocp_event_weak_ptr event_weak_ptr;

    typedef std::list<iocp_event_ptr>       event_list;

    typedef iocp_event::per_io_data         per_io_data;

    typedef std::vector<per_io_data *>      iocp_event_set;

public:
    iocp_poller    ();

    ~iocp_poller   () noexcept;

public:
    size_t
    wait           (iocp_event_set &events, timestamp ms);

    void
    wakeup         ();

public:
    void
    close          ();

    bool
    closed         () noexcept
    { return (INVALID_HANDLE_VALUE == iocp_handle_); }

public:
    void
    register_event (iocp_event_ptr e);

    void
    remove_event   (iocp_event &e);

    void
    modify_event   (iocp_event &e);

    bool
    registed       (iocp_event &e) noexcept;

private:
    size_t
    poll           (iocp_event_set &events, timestamp ms);

    size_t
    poll_wine      (iocp_event_set &events, timestamp ms);

private:
    event_list events_;

    mutex      mutex_;

    HANDLE     iocp_handle_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_IOCP_POLLER_H */

#endif /* KNGIN_FLAG_HAVE_EPOLLER */
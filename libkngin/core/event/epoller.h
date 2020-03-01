#ifndef KNGIN_EPOLLER_H
#define KNGIN_EPOLLER_H

#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_EPOLLER

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include <memory>
#include <vector>
#include <list>
#include "core/base/mutex.h"
#include "core/base/timestamp.h"
#include "core/event/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class epoller : public noncopyable {
public:
    typedef epoller_event::epoller_event_ptr      epoller_event_ptr;

    typedef epoller_event::epoller_event_weak_ptr epoller_event_weak_ptr;

    typedef std::list<epoller_event_ptr>          epoller_event_list;

    typedef std::vector<struct ::epoll_event>     epoll_event_set;

public:
    epoller        ();

    ~epoller       () noexcept;

public:
    size_t
    wait           (epoll_event_set &list, timestamp ms);

public:
    void
    close          ();

    bool
    closed         () noexcept
    { return epollfd_.invalid(); }

public:
    void
    register_event (epoller_event_ptr e);

    void
    remove_event   (epoller_event &e);

    void
    modify_event   (epoller_event &e);

    bool
    registed       (epoller_event &e) noexcept;

private:
    void
    update_event   (int opt, int fd, epoller_event *e);

private:
    epoller_event_list events_;

    mutex              mutex_;

    filefd             epollfd_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EPOLLER_H */

#endif /* KNGIN_FLAG_HAVE_EPOLLER */
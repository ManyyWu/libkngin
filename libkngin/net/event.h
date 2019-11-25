#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include <atomic>
#include "core/define.h"
#include "net/filefd.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event : public filefd {
public:
    typedef epoller_event::epoller_event_cb event_cb;

public:
    event        () = delete;

    explicit
    event        (event_loop *_loop);

    virtual
    ~event       ();

public:
    void
    start        (event_cb &&_cb);

    void
    update       ();

    void
    notify       ();

    void
    stop         ();

    epoller_event *
    get_event    () { return &m_event; }

    bool
    stopped      () { return m_stopped; }

protected:
    void
    on_event     ();

protected:
    event_loop *      m_loop;

    event_cb          m_event_cb;

    epoller_event     m_event;

    std::atomic<bool> m_stopped;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_H_ */
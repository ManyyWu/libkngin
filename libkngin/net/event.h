#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include <atomic>
#include "define.h"
#include "filefd.h"
#include "epoller_event.h"

__NAMESPACE_BEGIN

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

__NAMESPACE_END

#endif /* _EVENT_H_ */
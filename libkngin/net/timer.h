#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/timerfd.h>
#include "define.h"
#include "filefd.h"
#include "timestamp.h"
#include "epoller_event.h"

__NAMESPACE_BEGIN

class event_loop;
class timer : public filefd {
public:
    typedef epoller_event::epoller_event_cb timer_cb;

public:
    timer        () = delete;

    explicit
    timer        (event_loop *_loop);

    virtual
    ~timer       ();

public:
    void
    start        (timer_cb &&_timeout_cb, timestamp _val, timestamp _interval, bool _abs = false);

    void
    stop         ();

    timestamp
    get_time     ();

    void
    set_time     (timestamp _val, timestamp _interval, bool _abs = false);

    bool
    stopped      () { return m_stopped; }

    epoller_event *
    get_event    () { return &m_event; }

protected:
    void
    on_timeout   ();

protected:
    event_loop *      m_loop;

    timer_cb          m_timeout_cb;

    epoller_event     m_event;

    std::atomic<bool> m_stopped;
};

__NAMESPACE_END
#endif /* _TIMER_H_ */

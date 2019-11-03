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
    typedef std::function<void (timer &)> timer_cb;

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

public:

    void
    set_read_cb (timer_cb &&_cb);

    void
    set_write_cb (timer_cb &&_cb);

public:
    epoller_event *
    get_event    ();

    bool
    stopped      ();

protected:
    void
    set_time     (timestamp _val, timestamp _interval, bool _abs = false);

protected:
    void
    handle_read  ();

    void
    handle_write ();

protected:
    event_loop *      m_loop;

    timer_cb          m_timeout_cb;

    epoller_event     m_event;

    timer_cb          m_read_cb;

    timer_cb          m_write_cb;

    std::atomic<bool> m_stopped;
};

__NAMESPACE_END
#endif /* _TIMER_H_ */

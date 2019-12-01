#ifndef _TIMER_H_
#define _TIMER_H_

#include "core/define.h"
#include "core/timestamp.h"
#include "net/filefd.h"
#include "net/event_loop.h"

KNGIN_NAMESPACE_K_BEGIN

class timer : public filefd {
public:
    typedef event_loop::event_loop_pimpl_ptr event_loop_pimpl_ptr;

    typedef epoller_event::epoller_event_cb  timer_cb;

public:
    timer        () = delete;

    explicit
    timer        (event_loop_pimpl_ptr _loop);

    virtual
    ~timer       () KNGIN_NOEXP;

public:
    void
    start        (timer_cb &&_timeout_cb, timestamp _val, timestamp _interval,
                  bool _abs = false);

    void
    stop         ();

    timestamp
    get_time     ();

    void
    set_time     (timestamp _val, timestamp _interval, bool _abs = false);

    bool
    stopped      () KNGIN_NOEXP
    { return m_stopped; }

protected:
    void
    on_timeout   () KNGIN_NOEXP;

protected:
    event_loop_pimpl_ptr m_loop;

    timer_cb             m_timeout_cb;

    epoller_event        m_event;

    std::atomic<bool>    m_stopped;
};

KNGIN_NAMESPACE_K_END
#endif /* _TIMER_H_ */

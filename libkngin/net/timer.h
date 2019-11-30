#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include "core/define.h"
#include "core/timestamp.h"
#include "net/filefd.h"
#include "net/epoller_event.h"
#include "event_loop.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class timer : public filefd {
public:
    typedef event_loop::event_loop_pimpl_ptr event_loop_pimpl_ptr;

    typedef epoller_event::epoller_event_cb  timer_cb;

public:
    timer        () = delete;

    explicit
    timer        (event_loop_pimpl_ptr _loop) KNGIN_EXP;

    virtual
    ~timer       () KNGIN_NOEXP;

public:
    void
    start        (timer_cb &&_timeout_cb, timestamp _val, timestamp _interval,
                  bool _abs = false) KNGIN_EXP;

    void
    stop         () KNGIN_EXP;

    timestamp
    get_time     () KNGIN_EXP;

    void
    set_time     (timestamp _val, timestamp _interval, bool _abs = false) KNGIN_EXP;

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

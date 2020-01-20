#ifndef KNGIN_EVENT_LOOP_H
#define KNGIN_EVENT_LOOP_H

#include <deque>
#include <map>
#include <memory>
#include <functional>
#include <system_error>
#include "core/base/define.h"
#include "core/base/lock.h"
#include "core/base/noncopyable.h"
#include "core/base/thread.h"
#include "core/event/epoller.h"
#include "core/event/event.h"
#include "core/event/timer.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop : public noncopyable {
public:
    typedef epoller::epoller_event_ptr epoller_event_ptr;

    typedef epoller::epoll_event_set   epoll_event_set;

    typedef timer::timeout_handler     timeout_handler;

    typedef timer::timer_ptr           timer_ptr;

    typedef timer::timerid             timerid;

    typedef std::shared_ptr<barrier>   barrier_ptr;

    typedef std::function<void (void)> started_handler;

    typedef std::function<void (void)> stopped_handler;

    typedef std::function<void (void)> task;

    typedef std::deque<task>           taskq;

    typedef std::map<int, timer_ptr>   timers;

public:
    event_loop     ();

    ~event_loop    () KNGIN_NOEXCP;

public:
    void
    run            (started_handler &&_start_handler = nullptr,
                    stopped_handler &&_stop_handler = nullptr);

    void
    stop           ();

    bool
    looping        () KNGIN_NOEXCP
    { return m_looping; }

// event
public:
    void
    register_event (epoller_event_ptr _e);

    void
    remove_event   (epoller_event &_e);

    void
    update_event   (epoller_event &_e);

    bool
    registed       (epoller_event &_e);

// task
public:
    void
    run_in_loop    (task &&_fn);

// timer
public:
    void
    cancel         (const timer_ptr &_timer);

    void
    cancel         (timerid &_id);

    timerid
    run_after      (timestamp _delay, timeout_handler &&_handler,
                    bool _realtime = false);

    timerid
    run_every      (timestamp _interval, timeout_handler &&_handler,
                    bool _realtime = false);

    timerid
    run_at         (timestamp _absval, timeout_handler &&_handler,
                    bool _realtime = false);

public:
    bool
    in_loop_thread () const KNGIN_NOEXCP
    { return (m_tid == thread::tid()); }

protected:
    void
    wakeup         ();

protected:
    void
    add_timer      (timer_ptr &_timer);

private:
    uint64_t         m_tid;

    epoller          m_epoller;

    event::event_ptr m_waker;

    std::atomic_bool m_looping;

    std::atomic_bool m_stop;

    taskq            m_taskq;

    mutex            m_taskq_mutex;

    barrier_ptr      m_stop_barrier;

    epoll_event_set  m_events;

    timers           m_timers;

    mutex            m_timers_mutex;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */
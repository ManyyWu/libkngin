#ifndef KNGIN_EVENT_LOOP_H
#define KNGIN_EVENT_LOOP_H

#include <deque>
#include <map>
#include <list>
#include <memory>
#include <functional>
#include "core/base/define.h"
#include "core/base/lock.h"
#include "core/base/noncopyable.h"
#include "core/base/thread.h"
#ifdef KNGIN_FLAG_HAVE_EPOLLER
#include "core/event/epoller.h"
#else
#include "core/event/iocp_poller.h"
#endif
#include "core/event/event.h"
#include "core/event/timer.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop : public noncopyable {
public:
#ifdef KNGIN_FLAG_HAVE_EPOLLER
    typedef epoller_event               poller_event;

    typedef epoller::epoller_event_ptr  poller_event_ptr;

    typedef epoller::epoll_event_set    poller_event_set;

    typedef event::event_ptr            event_ptr;
#else
    typedef iocp_event                  poller_event;

    typedef iocp_poller::iocp_event_ptr poller_event_ptr;

    typedef iocp_poller::iocp_event_set poller_event_set;
#endif

    typedef timer::timeout_handler      timeout_handler;

    typedef timer::timer_ptr            timer_ptr;

    typedef timer::timer_weak_ptr       timer_weak_ptr;

    typedef timer::timerid              timerid;

    typedef std::shared_ptr<barrier>    barrier_ptr;

    typedef std::function<void (void)>  started_handler;

    typedef std::function<void (void)>  stopped_handler;

    typedef std::function<void (void)>  task;

    typedef std::deque<task>            taskq;

#if (ON == KNGIN_USE_TIMERFD)
    typedef std::map<int, timer_ptr>    timers;
#else
    typedef std::list<timer_ptr>        timers;
#endif

public:
    event_loop     ();

    ~event_loop    () noexcept;

public:
    void
    run            (started_handler &&_start_handler = nullptr,
                    stopped_handler &&_stop_handler = nullptr);

    void
    stop           ();

    bool
    looping        () noexcept
    { return m_looping; }

// event
public:
    void
    register_event (poller_event_ptr _e);
    void
    remove_event   (poller_event &_e);
    void
    update_event   (poller_event &_e);
    bool
    registed       (poller_event &_e);

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
    run_after      (timestamp _delay, timeout_handler &&_handler);

    timerid
    run_every      (timestamp _interval, timeout_handler &&_handler);

    timerid
    run_at         (timestamp _absval, timeout_handler &&_handler);

public:
    bool
    in_loop_thread () const noexcept
    { return (m_tid == thread::tid()); }

protected:
    void
    wakeup         ();

private:
    size_t
    io_pool        (poller_event_set &_events);

    void
    process_tasks  ();

#if (OFF == KNGIN_USE_TIMERFD)
    timestamp
    get_next_delay ();

    void
    process_timer  ();
#endif

void
    sort_events    (poller_event_set &_events, size_t _size);

    void
    process_events (poller_event_set &_events, size_t _size);

    void
    add_timer      (timer_ptr &_timer);

private:
    uint64_t         m_tid;

#ifdef KNGIN_FLAG_HAVE_EPOLLER
    epoller          m_poller;

    event_ptr        m_waker;
#else
    iocp_poller      m_poller;
#endif

    std::atomic_bool m_looping;

    std::atomic_bool m_stop;

    taskq            m_taskq;

    mutex            m_taskq_mutex;

    barrier_ptr      m_stop_barrier;

#if (OFF == KNGIN_USE_TIMERFD)
    timers           m_timers;

    mutex            m_timers_mutex;
#endif

    std::atomic_bool m_timer_processing;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */
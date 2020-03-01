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
    run            (started_handler &&start_handler = nullptr,
                    stopped_handler &&stop_handler = nullptr);

    void
    stop           ();

    bool
    looping        () noexcept
    { return looping_; }

// event
public:
    void
    register_event (poller_event_ptr e);
    void
    remove_event   (poller_event &e);
    void
    update_event   (poller_event &e);
    bool
    registed       (poller_event &e);

// task
public:
    void
    run_in_loop    (task &&fn);

// timer
public:
    void
    cancel         (const timer_ptr &timer);

    void
    cancel         (timerid &id);

    timerid
    run_after      (timestamp delay, timeout_handler &&handler);

    timerid
    run_every      (timestamp interval, timeout_handler &&handler);

    timerid
    run_at         (timestamp absval, timeout_handler &&handler);

public:
    bool
    in_loop_thread () const noexcept
    { return (tid_ == thread::tid()); }

protected:
    void
    wakeup         ();

private:
    size_t
    io_pool        (poller_event_set &events);

    void
    process_tasks  ();

#if (OFF == KNGIN_USE_TIMERFD)
    timestamp
    get_next_delay ();

    void
    process_timer  ();
#endif

void
    sort_events    (poller_event_set &events, size_t size);

    void
    process_events (poller_event_set &events, size_t size);

    void
    add_timer      (timer_ptr &timer);

private:
    uint64_t         tid_;

#ifdef KNGIN_FLAG_HAVE_EPOLLER
    epoller          poller_;

    event_ptr        waker_;
#else
    iocp_poller      poller_;
#endif

    std::atomic_bool looping_;

    std::atomic_bool stop_;

    taskq            taskq_;

    mutex            taskq_mutex_;

    barrier_ptr      stop_barrier_;

#if (OFF == KNGIN_USE_TIMERFD)
    timers           timers_;

    mutex            timers_mutex_;
#endif

    std::atomic_bool timer_processing_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */
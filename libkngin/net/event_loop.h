#ifndef KNGIN_EVENT_LOOP_H
#define KNGIN_EVENT_LOOP_H

#include <memory>
#include <functional>
#include <system_error>
#include <queue>
#include "core/define.h"
#include "core/lock.h"
#include "core/thread.h"
#include "core/noncopyable.h"
#include "net/epoller_event.h"
#include "net/epoller.h"
#include "net/event.h"

#ifndef NDEBUG
#define EPOLLER_TIMEOUT 3000
#else
#define EPOLLER_TIMEOUT 3000
#endif

KNGIN_NAMESPACE_K_BEGIN

class event_loop_pimpl
    : public noncopyable,
      public std::enable_shared_from_this<event_loop_pimpl> {
public:
    typedef std::function<void (void)> started_handler;

    typedef std::function<void (void)> stopped_handler;

    typedef std::function<void (void)> task;

    typedef event::event_ptr           waker_ptr;

    typedef epoller::epoller_event_ptr epoller_event_ptr;

    typedef thread::thread_pimpl_ptr   thread_pimpl_ptr;

public:
    event_loop_pimpl  ();

    explicit
    event_loop_pimpl  (thread &_thr);

    ~event_loop_pimpl () KNGIN_NOEXCP;

public:
    void
    run               (started_handler &&_start_handler,
                       stopped_handler &&_stop_handler);

    void
    stop              ();

    bool
    looping           () KNGIN_NOEXCP;

public:
    bool
    registed          (epoller_event &_e);

    void
    register_event    (epoller_event_ptr _e);

    void
    remove_event      (epoller_event &_e);

    void
    update_event      (epoller_event &_e);

    void
    run_in_loop       (task &&_fn);

public:
    void
    check_thread      () const KNGIN_NOEXCP;

    bool
    in_loop_thread    () const KNGIN_NOEXCP;

public:
    std::shared_ptr<event_loop_pimpl>
    self              ()
    { return shared_from_this(); }

protected:
    void
    wakeup            ();

private:
    thread_pimpl_ptr         m_thr;

    pthread_t                m_ptid;

    epoller                  m_epoller;

    waker_ptr                m_waker;

    std::atomic<bool>        m_looping;

    std::atomic<bool>        m_stop;

    std::queue<task>         m_taskq;

    mutex                    m_taskq_mutex;

    std::shared_ptr<barrier> m_stop_barrier;

    epoller::epoll_event_set m_events;
};

typedef std::shared_ptr<event_loop_pimpl>   event_loop_pimpl_ptr;

class event_loop : public noncopyable {
public:
    typedef event_loop_pimpl::epoller_event_ptr epoller_event_ptr;

    typedef event_loop_pimpl::started_handler   started_handler;

    typedef event_loop_pimpl::stopped_handler   stopped_handler;

    typedef event_loop_pimpl::task              task;

public:
    event_loop     ()
        : m_pimpl(std::make_shared<event_loop_pimpl>()) {}

    explicit
    event_loop     (thread &_thr)
        : m_pimpl(std::make_shared<event_loop_pimpl>(_thr)) {}

    ~event_loop    () = default;

public:
    void
    run            (started_handler &&_start_handler,
                    stopped_handler &&_stop_handler)
    { m_pimpl->run(std::move(_start_handler), std::move(_stop_handler)); }

    void
    stop           ()
    { m_pimpl->stop(); }

    bool
    looping        () KNGIN_NOEXCP
    { return m_pimpl->looping(); }

public:
    bool
    registed       (epoller_event &_e)
    { return m_pimpl->registed(_e); }

    void
    register_event (epoller_event_ptr _e)
    { m_pimpl->register_event(_e); }

    void
    remove_event   (epoller_event &_e)
    { m_pimpl->remove_event(_e); }

    void
    update_event   (epoller_event &_e)
    { m_pimpl->update_event(_e); }

    void
    run_in_loop    (task &&_fn)
    { m_pimpl->run_in_loop(std::move(_fn)); }

public:
    void
    check_thread   () const KNGIN_NOEXCP
    { m_pimpl->check_thread(); }

    bool
    in_loop_thread () const KNGIN_NOEXCP
    { return m_pimpl->in_loop_thread(); }

public:
    event_loop_pimpl_ptr &
    pimpl          ()
    { return m_pimpl; }

private:
    event_loop_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */
#ifndef KNGIN_EVENT_LOOP_H
#define KNGIN_EVENT_LOOP_H

#include <queue>
#include <memory>
#include <functional>
#include <system_error>
#include "core/define.h"
#include "core/lock.h"
#include "core/thread.h"
#include "core/noncopyable.h"
#include "net/epoller_event.h"
#include "net/epoller.h"
#include "net/event.h"
#include "net/timer.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop : public noncopyable {
public:
    typedef epoller::epoller_event_ptr epoller_event_ptr;

    typedef epoller::epoll_event_set   epoll_event_set;

    typedef timer::timeout_handler     timeout_handler;

    typedef std::shared_ptr<barrier>   barrier_ptr;

    typedef std::function<void (void)> started_handler;

    typedef std::function<void (void)> stopped_handler;

    typedef std::function<void (void)> task;

    typedef std::function<void (void)> slot;

    typedef std::queue<task>           taskq;

    typedef std::queue<slot>           slotq;

public:
    class pimpl
        : public noncopyable,
          public std::enable_shared_from_this<pimpl> {
    public:
        typedef std::shared_ptr<pimpl> pimpl_ptr;

        typedef std::weak_ptr<pimpl>   pimpl_weak_ptr;

    public:
        pimpl          ();

        explicit
        pimpl          (thread &_thr);

        ~pimpl         () KNGIN_NOEXCP;

    public:
        void
        run            (started_handler &&_start_handler,
                        stopped_handler &&_stop_handler);

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
        bool
        cancel         (timer::timerid &_id) = delete;

        bool
        expired        (timer::timerid &_id) = delete;

        timer::timerid
        run_after      (timestamp _delay, timeout_handler _handler) = delete;

        timer::timerid
        run_every      (timestamp _interval, timeout_handler _handler) = delete;

        timer::timerid
        run_at         (timestamp _absval, timeout_handler _handler) = delete;

    public:
        bool
        in_loop_thread () const KNGIN_NOEXCP;

    public:
        pimpl_ptr
        self           ()
        { return shared_from_this(); }

        pimpl_weak_ptr
        weak_self      ()
        { return shared_from_this(); }

    protected:
        void
        wakeup         ();

    private:
        thread::pimpl_weak_ptr m_thr;

        pthread_t              m_ptid;

        epoller                m_epoller;

        event::event_ptr       m_waker;

        std::atomic_bool       m_looping;

        std::atomic_bool       m_stop;

        taskq                  m_taskq;

        mutex                  m_taskq_mutex;

        barrier_ptr            m_stop_barrier;

        epoll_event_set        m_events;
    };

public:
    typedef pimpl::pimpl_ptr      pimpl_ptr;

    typedef pimpl::pimpl_weak_ptr pimpl_weak_ptr;

public:
    event_loop     ()
        : m_pimpl(std::make_shared<pimpl>()) {}

    explicit
    event_loop     (thread &_thr)
        : m_pimpl(std::make_shared<pimpl>(_thr)) {}

    ~event_loop    () = default;

public:
    void
    run            (started_handler &&_start_handler = nullptr,
                    stopped_handler &&_stop_handler = nullptr)
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
    bool
    in_loop_thread () const KNGIN_NOEXCP
    { return m_pimpl->in_loop_thread(); }

public:
    pimpl_weak_ptr
    weak_self      ()
    { return m_pimpl->weak_self(); }

private:
    pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */
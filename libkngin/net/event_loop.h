#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <memory>
#include <functional>
#include <deque>
#include "core/define.h"
#include "core/lock.h"
#include "core/thread.h"
#include "core/noncopyable.h"
#include "core/system_error.h"
#include "net/epoller_event.h"
#include "net/epoller.h"
#include "net/event.h"

#ifndef NDEBUG
#define EPOLLER_TIMEOUT 10000
#else
#define EPOLLER_TIMEOUT 3000
#endif

KNGIN_NAMESPACE_K_BEGIN

class event_loop : public noncopyable {
public:
    typedef std::function<void (std::error_code)> error_handler;

    typedef std::function<void (void)>            started_handler;

    typedef std::function<void (void)>            stopped_handler;

    typedef std::function<void (void)>            task;

    typedef event                                 waker;

protected:
    class pimpl
        : public noncopyable,
          public std::enable_shared_from_this<event_loop::pimpl> {
    public:
        typedef thread::pimpl        thread_pimpl;

        typedef epoller_event::pimpl epoller_event_pimpl;

    public:
        pimpl          () KNGIN_NOEXP = delete;

        explicit
        pimpl          (thread &_thr);

        ~pimpl         () KNGIN_NOEXP;

    public:
        void
        loop           (started_handler &&_start_cb, 
                        stopped_handler &&_stop_cb) KNGIN_EXP;

        void
        stop           () KNGIN_EXP;

        bool
        looping        () KNGIN_NOEXP;

    public:
        void
        add_event      (epoller_event &_e) KNGIN_EXP;

        void
        remove_event   (epoller_event &_e) KNGIN_EXP;

        void
        update_event   (epoller_event &_e) KNGIN_EXP;

        void
        run_in_loop    (task &&_fn) KNGIN_EXP;

    public:
        void
        check_thread   () const KNGIN_NOEXP;

        bool
        in_loop_thread () const KNGIN_NOEXP;

    public:
        std::shared_ptr<event_loop::pimpl>
        self           ()
        { return shared_from_this(); }

    protected:
        void
        wakeup         () KNGIN_EXP;

    private:
        thread_pimpl             m_thr_pimpl;

        epoller                  m_epoller;

        waker                    m_waker;

        std::atomic<bool>        m_looping;

        std::atomic<bool>        m_stop;

        std::deque<task>         m_taskq;

        mutex                    m_mutex;

        epoller::epoll_event_set m_events;
    };

public:
    typedef std::shared_ptr<event_loop::pimpl> event_loop_pimpl_ptr;

public:
    event_loop     () KNGIN_NOEXP = delete;

    explicit
    event_loop     (thread &_thr)
        : m_pimpl(std::make_shared<event_loop::pimpl>(_thr)) {}

    ~event_loop    () KNGIN_NOEXP = default;

public:
    void
    loop           (started_handler &&_start_cb, stopped_handler &&_stop_cb) KNGIN_EXP
    { m_pimpl->loop(std::move(_start_cb), std::move(_stop_cb)); }

    void
    stop           () KNGIN_EXP
    { m_pimpl->stop(); }

    bool
    looping        () KNGIN_NOEXP
    { return m_pimpl->looping(); }

public:
    void
    add_event      (epoller_event &_e) KNGIN_EXP
    { m_pimpl->add_event(_e); }

    void
    remove_event   (epoller_event &_e) KNGIN_EXP
    { m_pimpl->remove_event(_e); }

    void
    update_event   (epoller_event &_e) KNGIN_EXP
    { m_pimpl->update_event(_e); }

    void
    run_in_loop    (task &&_fn) KNGIN_EXP
    { m_pimpl->run_in_loop(std::move(_fn)); }

public:
    void
    check_thread   () const KNGIN_NOEXP
    { m_pimpl->check_thread(); }

    bool
    in_loop_thread () const KNGIN_NOEXP
    { m_pimpl->in_loop_thread(); }

public:
    event_loop_pimpl_ptr
    pimpl          ()
    { return m_pimpl; }

protected:
    event_loop_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_LOOP_H_ */
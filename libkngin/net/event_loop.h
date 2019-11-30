#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <memory>
#include <functional>
#include <system_error>
#include <deque>
#include "core/define.h"
#include "core/lock.h"
#include "core/thread.h"
#include "core/noncopyable.h"
#include "net/epoller_event.h"
#include "net/epoller.h"
#include "net/event.h"

#ifndef NDEBUG
#define EPOLLER_TIMEOUT 10000
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

    typedef std::shared_ptr<epoller>   epoller_ptr;

    typedef std::shared_ptr<event>     waker_ptr;

    typedef thread::thread_pimpl_ptr   thread_pimpl_ptr;

public:
    event_loop_pimpl  () = delete;

    explicit
    event_loop_pimpl  (thread &_thr) KNGIN_EXP;

    ~event_loop_pimpl () KNGIN_NOEXP;

public:
    void
    run               (started_handler &&_start_cb, stopped_handler &&_stop_cb) KNGIN_EXP;

    void
    stop              () KNGIN_EXP;

    bool
    looping           () KNGIN_NOEXP;

public:
    void
    add_event         (epoller_event &_e) KNGIN_EXP;

    void
    remove_event      (epoller_event &_e) KNGIN_EXP;

    void
    update_event      (epoller_event &_e) KNGIN_EXP;

    void
    run_in_loop       (task &&_fn) KNGIN_EXP;

public:
    void
    check_thread      () const KNGIN_NOEXP;

    bool
    in_loop_thread    () const KNGIN_NOEXP;

public:
    std::shared_ptr<event_loop_pimpl>
    self              () KNGIN_EXP
    { return shared_from_this(); }

protected:
    void
    wakeup            () KNGIN_EXP;

private:
    thread_pimpl_ptr         m_thr;

    epoller_ptr              m_epoller;

    waker_ptr                m_waker;

    std::atomic<bool>        m_looping;

    std::atomic<bool>        m_stop;

    std::deque<task>         m_taskq;

    mutex                    m_mutex;

    epoller::epoll_event_set m_events;
};

class event_loop : public noncopyable {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef event_loop_pimpl::started_handler started_handler;

    typedef event_loop_pimpl::stopped_handler stopped_handler;

    typedef event_loop_pimpl::task            task;

public:
    event_loop     () = delete;

    explicit
    event_loop     (thread &_thr) KNGIN_EXP
        : m_pimpl(std::make_shared<event_loop_pimpl>(_thr)) {}

    ~event_loop    () = default;

public:
    void
    run            (started_handler &&_start_cb, stopped_handler &&_stop_cb) KNGIN_EXP
    { m_pimpl->run(std::move(_start_cb), std::move(_stop_cb)); }

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
    { return m_pimpl->in_loop_thread(); }

public:
    event_loop_pimpl_ptr
    pimpl          () KNGIN_EXP
    { return m_pimpl; }

protected:
    event_loop_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_LOOP_H_ */
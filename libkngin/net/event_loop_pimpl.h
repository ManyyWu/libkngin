#ifndef _EVENT_LOOP_PIMPL_H_
#define _EVENT_LOOP_PIMPL_H_

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

class event_loop_pimpl;
class event_loop_pimpl
        : public noncopyable,
          public std::enable_shared_from_this<event_loop_pimpl> {
public:
    typedef std::function<void (void)>            started_handler;

    typedef std::function<void (void)>            stopped_handler;

    typedef std::function<void (void)>            task;

    typedef event                                 waker;

public:
    typedef thread::pimpl                         thread_pimpl;

public:
    event_loop_pimpl  () KNGIN_NOEXP = delete;

    explicit
    event_loop_pimpl  (thread &_thr);

    ~event_loop_pimpl () KNGIN_NOEXP;

public:
    void
    loop              (started_handler &&_start_cb,
                       stopped_handler &&_stop_cb) KNGIN_EXP;

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
    self              ()
    { return shared_from_this(); }

protected:
    void
    wakeup            () KNGIN_EXP;

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

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_LOOP_PIMPL_H_ */
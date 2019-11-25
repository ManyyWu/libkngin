#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include <deque>
#include "core/define.h"
#include "core/lock.h"
#include "core/thread.h"
#include "net/epoller.h"
#include "net/epoller_event.h"
#include "net/event.h"

#ifndef NDEBUG
#define EPOLLER_TIMEOUT 10000
#else
#define EPOLLER_TIMEOUT 3000
#endif

KNGIN_NAMESPACE_K_BEGIN

class event_loop {
public:
    typedef std::function<void (std::error_code)> error_cb;

    typedef std::function<void (void)>            started_cb;

    typedef std::function<void (void)>            stopped_cb;

    typedef std::function<void (void)>            task;

    typedef event                                 waker;

protected:
    class pimpl {

    };

    typedef std::shared_ptr<event_loop::pimpl> pimpl_ptr;

public:
    event_loop     () = delete;

    explicit
    event_loop     (thread *_thr);

    ~event_loop    ();

public:
    int
    loop           (started_cb &&_start_cb, stopped_cb &&_stop_cb);

    void
    stop           ();

    bool
    looping        ();

public:
    bool
    add_event      (epoller_event *_e);

    bool
    remove_event   (epoller_event *_e);

    bool
    update_event   (epoller_event *_e);

    void
    run_in_loop    (event_loop::task &&_fn);

public:
    void
    check_thread   () const;

    bool
    in_loop_thread () const;

protected:
    void
    wakeup         ();

protected:
    thread *                 m_thr;

    mutex                    m_mutex;

    epoller                  m_epoller;

    std::atomic<bool>        m_looping;

    std::atomic<bool>        m_stop;

    std::deque<task>         m_taskq;

    epoller::epoll_event_set m_events;

    waker                    m_waker;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_LOOP_H_ */
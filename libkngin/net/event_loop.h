#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include <deque>
#include "define.h"
#include "noncopyable.h"
#include "epoller.h"
#include "epoller_event.h"
#include "mutex.h"
#include "thread.h"
#include "event.h"
#include "timer.h"

#ifndef NDEBUG
#define EPOLLER_TIMEOUT 30000
#else
#define EPOLLER_TIMEOUT 3000
#endif

__NAMESPACE_BEGIN

class event_loop : noncopyable {
public:
    typedef std::function<void (void)> queued_fn;

    typedef event                      waker;

public:
    event_loop     () = delete;

    explicit
    event_loop     (thread *_thr);

    ~event_loop    ();

public:
    int
    loop           ();

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
    run_in_loop    (event_loop::queued_fn &&_fn);

public:
    void
    check_thread   ();

    bool
    in_loop_thread ();

protected:
    void
    wakeup         ();

    void
    handle_wakeup  ();

protected:
    waker                    m_waker;

    thread *                 m_thr;

    mutex                    m_mutex;

    epoller                  m_epoller;

    std::atomic<bool>        m_looping;

    std::atomic<bool>        m_stop;

    std::deque<queued_fn>    m_fnq;

    epoller::epoll_event_set m_events;
};

__NAMESPACE_END

#endif /* _EVENT_LOOP_H_ */
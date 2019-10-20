#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "epoller.h"
#include "epoller_event.h"
#include "mutex.h"
#include "thread.h"
#include "event.h"
#include "timer.h"

#define EPOLLER_TIMEOUT 3000

__NAMESPACE_BEGIN

class event_loop : noncopyable {
public:
    typedef event waker;

public:
    event_loop     () = delete;

    explicit
    event_loop     (thread *_thr);

    ~event_loop    ();

public:
    int
    loop           (void *_args);

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

protected:
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
    waker             m_waker;

    thread *          m_thr;

    mutex             m_mutex;

    epoller           m_epoller;

    std::atomic<bool> m_looping;

    std::atomic<bool> m_stop;
};

__NAMESPACE_END

#endif /* _EVENT_LOOP_H_ */
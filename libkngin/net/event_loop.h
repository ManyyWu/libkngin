#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "epoller_event.h"
#include "mutex.h"
#include "thread.h"

__NAMESPACE_BEGIN

class event_loop : noncopyable {
public:
    typedef int waker;

public:
    event_loop (thread *_thr);

    ~event_loop ();

public:
    void
    update_event  (epoller_event *_e) {};

    static event_loop::waker
    loop          (void *_args);

protected:
    static event_loop::waker
    create_waker  ();

    static void
    destroy_waker (event_loop::waker _waker);

protected:
    waker         m_waker_fd;

    thread *      m_thr;

    mutex         m_mutex;
};

__NAMESPACE_END

#endif /* _EVENT_LOOP_H_ */
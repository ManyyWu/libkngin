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

__NAMESPACE_BEGIN

class event_loop : noncopyable {
public:
    typedef event waker;

public:
    event_loop    (thread *_thr);

    ~event_loop   ();

public:
    void
    update_event  (epoller_event *_e);

    static int
    loop          (void *_args);

protected:
    void
    wakeup        ();

    void
    handle_wakeup ();

protected:
    waker         m_waker;

    epoller_event m_waker_event;

    thread *      m_thr;

    mutex         m_mutex;

    epoller       m_epoller;
};

__NAMESPACE_END

#endif /* _EVENT_LOOP_H_ */
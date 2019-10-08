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
    event_loop ();

    ~event_loop ();

public:
    void
    update_event (epoller_event *_e) {};

protected:
    epoller_event  m_wakeup_fd;

    thread         m_thr;

    mutex          m_mutex;
};

__NAMESPACE_END

#endif /* _EVENT_LOOP_H_ */
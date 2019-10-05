#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "epoller_event.h"

__NAMESPACE_BEGIN

class event_loop : noncopyable {
public:
    void
    update_event (epoller_event *_e);
};

__NAMESPACE_END

#endif /* _EVENT_LOOP_H_ */
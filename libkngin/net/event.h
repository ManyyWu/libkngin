#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class event_loop;
class event : noncopyable {
public:
    typedef std::function<void ()> event_cb;

public:
    event  (event_loop *_loop, event_cb &&_cb);

    ~event ();

protected:
    event_cb m_cb;
};

__NAMESPACE_END

#endif /* _EVENT_H_ */
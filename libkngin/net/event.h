#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "filefd.h"
#include "epoller_event.h"

__NAMESPACE_BEGIN

class event_loop;
class event : public filefd {
public:
    typedef std::function<void (event &)> event_cb;

public:
    event        (event_loop *_loop);

    virtual
    ~event       ();

public:
    void
    set_read_cb  (event_cb &&_cb);

    void
    set_write_cb (event_cb &&_cb);

public:
    epoller_event *
    get_event    ();

protected:
    event_loop *   m_loop;

    epoller_event  m_event;

    event_cb       m_read_cb;

    event_cb       m_write_cb;
};

__NAMESPACE_END

#endif /* _EVENT_H_ */
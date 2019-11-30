#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include <atomic>
#include "core/define.h"
#include "net/filefd.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event_loop_pimpl;
class event : public filefd {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef epoller_event::epoller_event_cb   event_cb;

public:
    event        () = delete;

    explicit
    event        (event_loop_pimpl_ptr _loop) KNGIN_EXP;

    virtual
    ~event       () KNGIN_NOEXP;

public:
    void
    start        (event_cb &&_cb) KNGIN_EXP;

    void
    update       () KNGIN_EXP;

    void
    notify       () KNGIN_EXP;

    void
    stop         () KNGIN_EXP;

    bool
    stopped      () KNGIN_NOEXP
    { return m_stopped; }

protected:
    void
    on_event     () KNGIN_NOEXP;

protected:
    event_loop_pimpl_ptr m_loop;

    event_cb             m_event_cb;

    epoller_event        m_event;

    std::atomic<bool>    m_stopped;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_H_ */
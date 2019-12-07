#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include <atomic>
#include "core/define.h"
#include "net/filefd.h"
#include "net/event_loop.h"

KNGIN_NAMESPACE_K_BEGIN

class event : public filefd {
public:
    typedef event_loop::event_loop_pimpl_ptr      event_loop_pimpl_ptr;

    typedef event_loop::epoller_event_ptr         epoller_event_ptr;

    typedef std::function<void (void)>            event_handler;

public:
    event        () = delete;

    explicit
    event        (event_loop_pimpl_ptr _loop);

    virtual
    ~event       () KNGIN_NOEXP;

public:
    void
    start        (event_handler &&_handler);

    void
    update       ();

    void
    notify       ();

    void
    notify       (std::error_code &_ec) KNGIN_NOEXP;

    void
    stop         ();

    bool
    stopped      () KNGIN_NOEXP
    { return m_stopped; }

private:
    void
    on_event     ();

private:
    event_loop_pimpl_ptr m_loop;

    event_handler        m_event_handler;

    epoller_event_ptr    m_event;

    std::atomic<bool>    m_stopped;
};

KNGIN_NAMESPACE_K_END

#endif /* _EVENT_H_ */
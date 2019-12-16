#ifndef KNGIN_TIMER_H
#define KNGIN_TIMER_H

#include <memory>
#include <functional>
#include "core/define.h"
#include "core/timestamp.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class event_loop_pimpl;
class timer
    : public epoller_event,
      public std::enable_shared_from_this<timer> {
public:
    typedef std::shared_ptr<event_loop_pimpl> event_loop_pimpl_ptr;

    typedef std::function<void (void)>        timeout_handler;

    typedef std::shared_ptr<timer>            timer_ptr;

public:
    timer    () = delete;

    timer    (event_loop_pimpl_ptr &_loop,
              timeout_handler &&_timeout_handler);

    virtual
    ~timer   () KNGIN_NOEXCP;

public:
    timestamp
    get_time ();

    void
    set_time (timestamp _val, timestamp _interval, bool _abs = false);

    void
    close    ();

public:
    event_loop_pimpl_ptr &
    loop     () KNGIN_NOEXCP
    { return m_loop; }

    timer_ptr
    self     ()
    { return shared_from_this(); }

private:
    virtual void
    on_read  ();

    virtual void
    on_error ();

private:
    event_loop_pimpl_ptr m_loop;

    timeout_handler      m_timeout_handler;

private:
    friend class epoller;
};

KNGIN_NAMESPACE_K_END
#endif /* KNGIN_TIMER_H */

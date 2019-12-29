#ifndef KNGIN_TIMER_H
#define KNGIN_TIMER_H

#include <memory>
#include <functional>
#include "core/define.h"
#include "core/timestamp.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

class timer
    : public epoller_event,
      public std::enable_shared_from_this<timer> {
public:
    typedef std::function<void (void)> timeout_handler;

    typedef std::shared_ptr<timer>     timer_ptr;

    typedef std::weak_ptr<timer>       timer_weak_ptr;

public:
    class timerid {
    public:
        timerid (timer_ptr _timer)
            : m_timer(_timer) {}

    public:
        bool
        expired () const KNGIN_NOEXCP
        { return m_timer.expired(); }

    private:
        timer_weak_ptr m_timer;
    };

public:
    timer    () = delete;

    timer    (timeout_handler &&_handler, bool _abs  = false);

    virtual
    ~timer   () KNGIN_NOEXCP;

public:
    timestamp
    get_time ();

    void
    set_time (timestamp _val, timestamp _interval,
              bool _abs = false);

    void
    close    ();

public:
    timer_ptr
    self     ()
    { return shared_from_this(); }

private:
    virtual void
    on_read  ();

    virtual void
    on_error ();

private:
    timeout_handler m_timeout_handler;

private:
    friend class epoller;
};

KNGIN_NAMESPACE_K_END
#endif /* KNGIN_TIMER_H */

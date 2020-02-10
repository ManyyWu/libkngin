#ifndef KNGIN_TIMER_H
#define KNGIN_TIMER_H

#include <cassert>
#include <memory>
#include <functional>
#include "core/base/define.h"
#include "core/event/epoller.h"
#include "core/base/timeout.h"

KNGIN_NAMESPACE_K_BEGIN

#if (ON == KNGIN_USE_TIMERFD)
class event_loop;
class timer
    : public epoller_event,
      public std::enable_shared_from_this<timer> {
    friend class event_loop;

public:
    typedef std::shared_ptr<timer>                  timer_ptr;

    typedef std::weak_ptr<timer>                    timer_weak_ptr;

    typedef std::function<void (const timer_ptr &)> timeout_handler;

public:
    class timerid {
    public:
        timerid    (timer_ptr _timer);

        timerid    (const timerid &_timer);

        ~timerid   () = default;

    public:
        timerid &
        operator = (const timerid &_timer);

    public:
        timestamp
        interval   () const KNGIN_NOEXCP;

        bool
        realtime   () const KNGIN_NOEXCP;

        bool
        abs        () const KNGIN_NOEXCP;

        bool
        cancelled  ()
        { return bool(m_timer.expired()); }

    public:
        int
        key        () const KNGIN_NOEXCP;

        timer_weak_ptr
        weak_ptr   ()
        { return m_timer; }

    private:
        timer_weak_ptr m_timer;
    };

public:
    timer     () = delete;

    timer     (timeout_handler &&_handler, bool _realtime  = false);

    virtual
    ~timer    () KNGIN_NOEXCP;

public:
    void
    set_time  (timestamp _val, timestamp _interval,
               bool _abs = false);

    void
    close     ();

public:
    timer_ptr
    self      ()
    { return shared_from_this(); }

    int
    key       () const KNGIN_NOEXCP
    { return fd(); }

private:
    virtual void
    on_events (event_loop &_loop, uint32_t _flags);

    void
    on_read   (event_loop &_loop);

private:
    timeout_handler m_timeout_handler;

    timestamp       m_initval;

    timestamp       m_interval;

    bool            m_realtime;

    bool            m_abs;
};

#else

class event_loop;
class timer
    : public std::enable_shared_from_this<timer> {
    friend class event_loop;

public:
    typedef std::shared_ptr<timer>                  timer_ptr;

    typedef std::weak_ptr<timer>                    timer_weak_ptr;

    typedef std::function<void (const timer_ptr &)> timeout_handler;

public:
    class timerid {
    public:
        timerid    (timer_ptr _timer)
            : m_timer(_timer) {}

        timerid    (const timerid &_timer)
            : m_timer(_timer.m_timer) {}

        ~timerid   () = default;

    public:
        timerid &
        operator = (const timerid &_timer)
        { m_timer = _timer.m_timer; return *this; }

    public:
        timestamp
        interval   () const KNGIN_NOEXCP
        { if (auto _timer = m_timer.lock()) return assert(_timer), _timer->m_timeout.interval(); return 0ULL; }

        bool
        persist    () const KNGIN_NOEXCP
        { if (auto _timer = m_timer.lock()) return assert(_timer), _timer->m_timeout.persist(); return false; }

        bool
        cancelled  ()
        { return bool(m_timer.expired()); }

    public:
        timer_weak_ptr
        weak_ptr   ()
        { return m_timer; }

    private:
        timer_weak_ptr m_timer;
    };

public:
    timer     () = delete;

    timer     (timeout_handler &&_handler);

    virtual
    ~timer    () = default;

public:
    void
    set_time  (timestamp _cur_time, timestamp _delay, bool _persist);

    void
    close     ()
    { m_timeout.clear(); m_closed = true; }

    bool
    closed    () const KNGIN_NOEXCP
    { return m_closed; }

private:
    timer_ptr
    self      ()
    { return shared_from_this(); }

private:
    void
    on_events (event_loop &_loop, timestamp _now_time);

private:
    timeout_handler  m_timeout_handler;

    timeout          m_timeout;

    std::atomic_bool m_closed;
};

#endif

KNGIN_NAMESPACE_K_END
#endif /* KNGIN_TIMER_H */

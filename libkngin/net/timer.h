#ifndef KNGIN_TIMER_H
#define KNGIN_TIMER_H

#include <memory>
#include <functional>
#include "core/define.h"
#include "core/timestamp.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN

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

    public:
        timerid &
        operator = (const timerid &_timer);

    public:
        timestamp
        initval    () const KNGIN_NOEXCP
        { return m_initval; }

        timestamp
        interval   () const KNGIN_NOEXCP
        { return m_interval; }

        bool
        realtime   () const KNGIN_NOEXCP
        { return m_realtime; }

        bool
        abs        () const KNGIN_NOEXCP
        { return m_abs; }

        bool
        cancelled  ()
        { auto _timer = m_timer.lock(); return (_timer and _timer->registed()); }

    public:
        int
        key        () const KNGIN_NOEXCP
        { return m_key; }

        timer_weak_ptr
        weak_ptr   ()
        { return m_timer; }

    private:
        timer_weak_ptr m_timer;

        size_t         m_key;

        timestamp      m_initval;

        timestamp      m_interval;

        bool           m_realtime;

        bool           m_abs;

        bool           m_persist;
    };

public:
    timer     () = delete;

    timer     (timeout_handler &&_handler, bool _realtime  = false);

    virtual
    ~timer    () KNGIN_NOEXCP;

public:
    timestamp
    get_time  ();

    void
    set_time  (timestamp _val, timestamp _interval,
               bool _abs = false, bool _persist = false);

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

    bool            m_persist;
};

KNGIN_NAMESPACE_K_END
#endif /* KNGIN_TIMER_H */

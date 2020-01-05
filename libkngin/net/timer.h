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
        bool
        cancelled  ();

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
               bool _abs = false);

    void
    close     ();

public:
    bool
    cancelled ()
    { return !registed(); }

    timer_ptr
    self      ()
    { return shared_from_this(); }

    int
    key       () const KNGIN_NOEXCP
    { return fd(); }

    bool
    repeat    () const KNGIN_NOEXCP
    { return m_interval.value(); }

private:
    virtual void
    on_read   ();

    virtual void
    on_error  ();

private:
    timeout_handler m_timeout_handler;

    timestamp      m_initval;

    timestamp      m_interval;

    bool           m_realtime;

    bool           m_abs;

private:
    friend class epoller;
};

KNGIN_NAMESPACE_K_END
#endif /* KNGIN_TIMER_H */

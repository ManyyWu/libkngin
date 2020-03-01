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
        timerid    (timer_ptr timer);

        timerid    (const timerid &timer);

        ~timerid   () = default;

    public:
        timerid &
        operator = (const timerid &timer);

    public:
        timestamp
        interval   () const noexcept;

        bool
        abs        () const noexcept;

        bool
        cancelled  ()
        { return bool(timer_.expired()); }

    public:
        int
        key        () const noexcept;

        timer_weak_ptr
        weak_ptr   ()
        { return timer_; }

    private:
        timer_weak_ptr timer_;
    };

public:
    timer     () = delete;

    timer     (timeout_handler &&handler);

    virtual
    ~timer    () noexcept;

public:
    void
    set_time  (timestamp val, timestamp interval,
               bool abs = false);

    void
    close     ();

public:
    timer_ptr
    self      ()
    { return shared_from_this(); }

    int
    key       () const noexcept
    { return fd(); }

private:
    virtual void
    on_events (event_loop &loop, uint32_t flags);

    void
    on_read   (event_loop &loop);

private:
    timeout_handler timeout_handler_;

    timestamp       initval_;

    timestamp       interval_;

    bool            abs_;
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
        timerid    (timer_ptr timer)
            : timer_(timer) {}

        timerid    (const timerid &timer)
            : timer_(timer.timer_) {}

        ~timerid   () = default;

    public:
        timerid &
        operator = (const timerid &timer)
        { timer_ = timer.timer_; return *this; }

    public:
        timestamp
        interval   () const noexcept
        { if (auto timer = timer_.lock()) return assert(timer), timer->timeout_.interval(); return 0ULL; }

        bool
        persist    () const noexcept
        { if (auto timer = timer_.lock()) return assert(timer), timer->timeout_.persist(); return false; }

        bool
        cancelled  ()
        { return bool(timer_.expired()); }

    public:
        timer_weak_ptr
        weak_ptr   ()
        { return timer_; }

    private:
        timer_weak_ptr timer_;
    };

public:
    timer     () = delete;

    timer     (timeout_handler &&handler);

    virtual
    ~timer    () = default;

public:
    void
    set_time  (timestamp cur_time, timestamp delay, bool persist);

    void
    close     ()
    { timeout_.clear(); closed_ = true; }

    bool
    closed    () const noexcept
    { return closed_; }

private:
    timer_ptr
    self      ()
    { return shared_from_this(); }

private:
    void
    on_events (event_loop &loop);

private:
    timeout_handler  timeout_handler_;

    timeout          timeout_;

    std::atomic_bool closed_;
};

#endif

KNGIN_NAMESPACE_K_END
#endif /* KNGIN_TIMER_H */

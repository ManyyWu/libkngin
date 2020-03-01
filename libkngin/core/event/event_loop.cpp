#ifdef _WIN32
#else
#include <unistd.h>
#include <sys/eventfd.h>
#endif
#include <algorithm>
#include "core/base/bits.h"
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/base/thread.h"
#include "core/event/event_loop.h"
#include "core/event/event.h"

#define KNGIN_FILENAME "libkngin/core/event/event_loop.cpp"

KNGIN_NAMESPACE_K_BEGIN

event_loop::event_loop ()
    try
    : tid_(0),
      poller_(),
#ifdef KNGIN_FLAG_HAVE_EPOLLER
      waker_(nullptr),
#endif
      looping_(false),
      stop_(false),
      taskq_(),
      taskq_mutex_(),
      stop_barrier_(std::make_shared<barrier>(2)),
#if (OFF == KNGIN_USE_TIMERFD)
      timers_mutex_(),
#endif
      timer_processing_(false)
{
} catch (...) {
    log_fatal("event_loop::pimpl() error");
    throw;
}

event_loop::~event_loop () noexcept
{
    if (looping_)
        ignore_excp(stop());
}

void
event_loop::run (started_handler &&start_handler, stopped_handler &&stop_handler)
{
    bool _throw = false;
    tid_ = thread::tid();
    looping_ = true;
    log_info("event loop is running in thread %" PRIu64, thread::tid());

    auto fail = [&] () {
#ifdef KNGIN_FLAG_HAVE_EPOLLER
        if (waker_) {
            if (waker_->registed())
                remove_event(*waker_);
            waker_->close();
            waker_.reset();
        }
#endif
        if (stop_handler) {
            log_excp_error(
                stop_handler(),
                "stop_handler() error"
            );
        }
        looping_ = false;
        if (_throw) {
            if (!stop_barrier_->destroyed())
                stop_barrier_->destroy();
            log_fatal("caught an exception in event loop of thread %" PRIu64, thread::tid());
            throw;
        }

        auto temp_ptr = stop_barrier_;
        if (!temp_ptr->destroyed() and temp_ptr->wait())
            temp_ptr->destroy();
    };

    try {
        poller_event_set events(KNGIN_RESERVED_POLLELR_EVENT);
#ifdef KNGIN_FLAG_HAVE_EPOLLER
        waker_ = std::make_shared<event>([] () {});
        register_event(waker_);
#endif
        if (start_handler) {
            log_excp_error(
                start_handler(),
                "start_handler() error"
            );
        }

        while (!stop_) {
            // process queued tasks
            process_tasks();

            // wait for events
            size_t size = io_pool(events);
            if (stop_)
                break;

            // process timer
#if (OFF == KNGIN_USE_TIMERFD)
            process_timer();
#endif

            // sort the events by priority and type(timer > event > file)
            sort_events(events, size);

            // process events
            process_events(events, size);
        }
    } catch (...) {
        _throw = true;
        fail();
    }
    fail();
}

void
event_loop::stop ()
{
    if (!looping_)
        return;

    stop_ = true;
    if (in_loop_thread()) {
        if (!stop_barrier_->destroyed())
            stop_barrier_->destroy();
    } else {
        wakeup();
        std::shared_ptr<barrier> temp_ptr = stop_barrier_;
        if (temp_ptr->wait())
            temp_ptr->destroy();
    }
    {
        local_lock lock(taskq_mutex_);
        taskq_.clear();
    }
}

void
event_loop::register_event (poller_event_ptr e)
{
    poller_.register_event(e);
    if (looping_ and !in_loop_thread())
        wakeup();
}

void
event_loop::remove_event (poller_event &e)
{
    poller_.remove_event(e);
    if (looping_ and !in_loop_thread())
        wakeup();
}

void
event_loop::update_event (poller_event &e)
{
    poller_.modify_event(e);
    if (looping_ and !in_loop_thread())
        wakeup();
}

bool
event_loop::registed (poller_event &e)
{
    return poller_.registed(e);
}

void
event_loop::run_in_loop (event_loop::task &&fn)
{
    if (fn)
    {
        local_lock lock(taskq_mutex_);
        taskq_.push_front(std::move(fn));
    }
    if (looping_ and !in_loop_thread())
        wakeup();
}

void
event_loop::cancel (const timer_ptr &timer)
{
    assert(timer);
#if (ON == KNGIN_USE_TIMERFD)
    if (timer->registed())
    {
        remove_event(*timer);
        timer->close();
    }
#else
    if (timer_processing_) {
        timer->close(); // remove on the next processing
    } else {
        local_lock lock(timers_mutex_);
        timer->close();
        timers_.remove(timer);
    }
#endif
}

void
event_loop::cancel (timer::timerid &id) {
    auto timer = id.weak_ptr().lock();
    if (!timer)
        return;
#if (ON == KNGIN_USE_TIMERFD)
    if (timer->registed())
    {
        remove_event(*timer);
        timer->close();
    }
#else
    if (timer_processing_) {
        timer->close(); // remove on the next processing
    } else {
        local_lock lock(timers_mutex_);
        timer->close();
        timers_.remove(timer);
    }
#endif
}

timer::timerid
event_loop::run_after (timestamp delay, timeout_handler &&handler)
{
    assert(delay);
#if (ON == KNGIN_USE_TIMERFD)
    auto t= std::make_shared<timer>(std::move(handler));
    t->set_time(delay, 0, false);
#else
    auto t = std::make_shared<timer>(std::move(handler));
    t->set_time(timestamp::monotonic(), delay, false);
#endif
    add_timer(t);
    return timerid(t);
}

timer::timerid
event_loop::run_every (timestamp interval, timeout_handler &&handler)
{
    assert(interval);
#if (ON == KNGIN_USE_TIMERFD)
    auto t= std::make_shared<timer>(std::move(handler));
    t->set_time(interval, interval, false);
#else
    auto t= std::make_shared<timer>(std::move(handler));
    t->set_time(timestamp::monotonic(), interval, true);
#endif
    add_timer(t);
    return timerid(t);
}

timer::timerid
event_loop::run_at (timestamp absval, timeout_handler &&handler)
{
    assert(absval);
#if (ON == KNGIN_USE_TIMERFD)
    auto t= std::make_shared<timer>(std::move(handler));
    t->set_time(timestamp::diff(absval, timestamp::realtime()), 0, false);
#else
    auto t= std::make_shared<timer>(std::move(handler));
    auto now_time = timestamp::monotonic();
    t->set_time(now_time, timestamp::diff(absval, timestamp::realtime()), false);
#endif
    add_timer(t);
    return timerid(t);
}

size_t
event_loop::io_pool (poller_event_set &events)
{
#if (OFF == KNGIN_USE_TIMERFD)
    auto delay = get_next_delay();
#else
    static auto delay = timestamp(KNGIN_DEFAULT_POLLER_TIMEOUT);
#endif
    return poller_.wait(events, delay);
}

void
event_loop::process_tasks ()
{
    std::deque<task> taskq;
    {
        local_lock lock(taskq_mutex_);
        if (!taskq_.empty())
            taskq.swap(taskq_);
    }
    while (!taskq.empty()) {
        taskq.back()();
        taskq.pop_back();
    }
}

#if (OFF == KNGIN_USE_TIMERFD)
timestamp
event_loop::get_next_delay ()
{
    auto get_min_delay = [this] () -> timestamp {
        timestamp now_time = timestamp::monotonic();
        timestamp min = KNGIN_DEFAULT_POLLER_TIMEOUT;
        for (auto &iter : timers_) {
            auto remaining = iter->timeout_.remaining(now_time);
            if (remaining < min)
                min = remaining;
        }
        server_debug("next delay: %" PRIu64, min.value());
        return min;
    };

    if (timer_processing_ and in_loop_thread()) {
        return get_min_delay();
    } else {
        local_lock lock(timers_mutex_);
        return get_min_delay();
    }
}

void
event_loop::process_timer ()
{
    timer_processing_ = true;
    {
        local_lock lock(timers_mutex_);
        for (auto iter = timers_.begin(); iter != timers_.end(); ++iter) {
            if (timers_.empty() or stop_)
                break;
            timer_ptr timer = *iter;
            assert(timer);
            auto now_time = timestamp::monotonic();
            auto remaining = timer->timeout_.remaining(now_time);

            // remove closed timer
            if (timer->closed())
                goto remove;

            // handle timer
            if (remaining  > timestamp(KNGIN_TIMER_REMAINING_PRESISION))
                continue;
            log_excp_error(
                timer->on_events(*this),
                "poller_event_handler::on_events() error"
            );

            if (!timer->closed() and !timer->timeout_.persist()) {
                // remove once timer
remove:
                timer->close();
                iter = timers_.erase(iter);
                if (iter == timers_.end())
                    break;
            } else {
                // update
                timer->timeout_.update(now_time);
            }
        }
    }
    timer_processing_ = false;
}
#endif /* (OFF == KNGIN_USE_TIMERFD) */

void
event_loop::sort_events (poller_event_set &events, size_t size)
{
#ifdef _WIN32
#else
    std::sort(events.begin(), events.begin() + size,
        [] (struct ::epoll_event &e1, struct ::epoll_event &e2) -> bool {
        epoller_event *ptr1 = static_cast<epoller_event *>(e1.data.ptr);
        epoller_event *ptr2 = static_cast<epoller_event *>(e2.data.ptr);
        return (ptr1->type() > ptr2->type() or
                (ptr1->type() == ptr2->type() and
                 ptr1->priority() > ptr2->priority()));
    }); // end of operator < for sortting
#endif
}

void
event_loop::process_events (poller_event_set &events, size_t size)
{
    for (uint32_t i = 0; i < size and !stop_; i++) {
#ifdef _WIN32
#else
        auto *ptr = static_cast<poller_event *>(events[i].data.ptr);
        assert(ptr);
        assert(ptr->registed());
        if (ptr->registed()) {
            log_excp_error(
                ptr->on_events(*this, events[i].events),
                "epoller_event_handler::on_events() error"
            );
        }
#endif
    }
}

void
event_loop::wakeup ()
{
#ifdef KNGIN_FLAG_HAVE_EPOLLER
    if (waker_ and looping_ and waker_->registed())
        waker_->notify(); // blocked
#else
    poller_.wakeup();
#endif
}

void
event_loop::add_timer (timer_ptr &timer)
{
#if (ON == KNGIN_USE_TIMERFD)
    register_event(timer);
#else
    if (timer_processing_ and looping_ and in_loop_thread()) {
        timers_.push_front(timer);
    } else {
        local_lock lock(timers_mutex_);
        timers_.push_front(timer);
    }
    wakeup();
#endif
}

KNGIN_NAMESPACE_K_END

#ifdef _WIN32
#else
#include <unistd.h>
#include <sys/eventfd.h>
#endif
#include <algorithm>
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/base/thread.h"
#include "core/event/event_loop.h"
#include "core/event/event.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/event/event_loop.cpp"

KNGIN_NAMESPACE_K_BEGIN

event_loop::event_loop ()
    try
    : m_tid(0),
      m_epoller(),
      m_waker(nullptr),
      m_looping(false),
      m_stop(false),
      m_taskq(),
      m_taskq_mutex(),
      m_stop_barrier(std::make_shared<barrier>(2)),
      m_timers_mutex(),
      m_timer_processing(false)
{
} catch (...) {
    log_fatal("event_loop::pimpl() error");
    throw;
}

event_loop::~event_loop () KNGIN_NOEXCP
{
    if (m_looping)
        ignore_excp(stop());
}

void
event_loop::run (started_handler &&_start_handler, stopped_handler &&_stop_handler)
{
    bool _throw = false;
    m_tid = thread::tid();
    m_looping = true;
    log_info("event loop is running in thread %" PRIu64, thread::tid());

    auto _fail = [&] () {
        if (m_waker) {
            if (m_waker->registed())
                remove_event(*m_waker);
            m_waker->close();
            m_waker.reset();
        }
        if (_stop_handler) {
            log_excp_error(
                _stop_handler(),
                "stop_handler() error"
            );
        }
        m_looping = false;
        if (_throw) {
            if (!m_stop_barrier->destroyed())
                m_stop_barrier->destroy();
            log_fatal("caught an exception in event loop of thread %" PRIu64, thread::tid());
            throw;
        }

        auto _temp_ptr = m_stop_barrier;
        if (!_temp_ptr->destroyed() and _temp_ptr->wait())
            _temp_ptr->destroy();
    };

    try {
        epoll_event_set _events(KNGIN_RESERVED_EPOLLELR_EVENT);
        m_waker = std::make_shared<event>([] () {});
        register_event(m_waker);
        if (_start_handler) {
            log_excp_error(
                _start_handler(),
                "start_handler() error"
            );
        }

        while (!m_stop) {
            // process queued tasks
            process_tasks();

            // wait for events

            size_t _size = io_pool(_events);
            if (m_stop)
                break;

            // process timer
#if (OFF == KNGIN_USE_TIMERFD)
            process_timer();
#endif

#ifndef _WIN32
            // sort the events by priority and type(timer > event > file)
            sort_events(_events, _size);

            // process events
            process_events(_events, _size);
#endif
        }
    } catch (...) {
        _throw = true;
        _fail();
    }
    _fail();
}

void
event_loop::stop ()
{
    if (!m_looping)
        return;

    m_stop = true;
    if (in_loop_thread()) {
        if (!m_stop_barrier->destroyed())
            m_stop_barrier->destroy();
    } else {
        wakeup();
        std::shared_ptr<barrier> _temp_ptr = m_stop_barrier;
        if (_temp_ptr->wait())
            _temp_ptr->destroy();
    }
    {
        local_lock _lock(m_taskq_mutex);
        m_taskq.clear();
    }
}

void
event_loop::register_event (epoller_event_ptr _e)
{
    m_epoller.register_event(_e);
    if (m_looping and !in_loop_thread())
        wakeup();
}

void
event_loop::remove_event (epoller_event &_e)
{
    m_epoller.remove_event(_e);
    if (m_looping and !in_loop_thread())
        wakeup();
}

void
event_loop::update_event (epoller_event &_e)
{
    m_epoller.modify_event(_e);
    if (m_looping and !in_loop_thread())
        wakeup();
}

bool
event_loop::registed (epoller_event &_e)
{
    return m_epoller.registed(_e);
}

void
event_loop::run_in_loop (event_loop::task &&_fn)
{
    if (_fn)
    {
        local_lock _lock(m_taskq_mutex);
        m_taskq.push_front(std::move(_fn));
    }
    if (m_looping and !in_loop_thread())
        wakeup();
}

void
event_loop::cancel (const timer_ptr &_timer)
{
    assert(_timer);
#if (ON == KNGIN_USE_TIMERFD)
    if (_timer->registed())
    {
        local_lock _lock(m_timers_mutex);
        remove_event(*_timer);
        _timer->close();
    }
#else
    if (m_timer_processing) {
        _timer->close(); // remove on the next processing
    } else {
        local_lock _lock(m_timers_mutex);
        _timer->close();
        m_timers.remove(_timer);
    }
#endif
}

void
event_loop::cancel (timer::timerid &_id) {
    auto _timer = _id.weak_ptr().lock();
    if (!_timer)
        return;
#if (ON == KNGIN_USE_TIMERFD)
    if (_timer->registed())
    {
        local_lock _lock(m_timers_mutex);
        remove_event(*_timer);
        _timer->close();
    }
#else
    if (m_timer_processing) {
        _timer->close(); // remove on the next processing
    } else {
        local_lock _lock(m_timers_mutex);
        _timer->close();
        m_timers.remove(_timer);
    }
#endif
}

timer::timerid
event_loop::run_after (timestamp _delay, timeout_handler &&_handler,
                       bool _realtime /* = false */)
{
    assert(_delay);
#if (ON == KNGIN_USE_TIMERFD)
    auto _timer = std::make_shared<timer>(std::move(_handler), _realtime);
    _timer->set_time(_delay, 0, false);
#else
    auto _timer = std::make_shared<timer>(std::move(_handler));
    _timer->set_time(timestamp::monotonic(), _delay, false);
#endif
    add_timer(_timer);
    return timerid(_timer);
}

timer::timerid
event_loop::run_every (timestamp _interval, timeout_handler &&_handler,
                       bool _realtime /* = false */)
{
    assert(_interval);
#if (ON == KNGIN_USE_TIMERFD)
    auto _timer = std::make_shared<timer>(std::move(_handler), _realtime);
    _timer->set_time(_interval, _interval, false);
#else
    auto _timer = std::make_shared<timer>(std::move(_handler));
    _timer->set_time(timestamp::monotonic(), _interval, true);
#endif
    add_timer(_timer);
    return timerid(_timer);
}

timer::timerid
event_loop::run_at (timestamp _absval, timeout_handler &&_handler,
                    bool _realtime /* = false */)
{
    assert(_absval);
#if (ON == KNGIN_USE_TIMERFD)
    auto _timer = std::make_shared<timer>(std::move(_handler), _realtime);
    _timer->set_time(_absval, 0, true);
#else
    auto _timer = std::make_shared<timer>(std::move(_handler));
    auto _now_time = timestamp::monotonic();
    _timer->set_time(_now_time, (_now_time > _absval
                                ? timestamp(0)
                                : timestamp::diff(_absval, _now_time)),
                     false);
#endif
    add_timer(_timer);
    return timerid(_timer);
}

size_t
event_loop::io_pool (epoll_event_set &_events)
{
#if (OFF == KNGIN_USE_TIMERFD)
    auto _delay = get_next_delay();
#else
    static auto _delay = timestamp(KNGIN_DEFAULT_POLLER_TIMEOUT);
#endif
#ifdef _WIN32
    size_t _size = 0;

    return _size;
#else
    return m_epoller.wait(_events, _delay);
#endif
}

void
event_loop::process_tasks ()
{
    std::deque<task> _taskq;
    {
        local_lock _lock(m_taskq_mutex);
        if (!m_taskq.empty())
            _taskq.swap(m_taskq);
    }
    while (!_taskq.empty()) {
        _taskq.back()();
        _taskq.pop_back();
    }
}

#if (OFF == KNGIN_USE_TIMERFD)
timestamp
event_loop::get_next_delay ()
{
    auto _get_min_delay = [this] () -> timestamp {
        timestamp _now_time = timestamp::monotonic();
        timestamp _min = KNGIN_DEFAULT_POLLER_TIMEOUT;
        for (auto &_iter : m_timers) {
            auto _remaining = _iter->m_timeout.remaining(_now_time);
            if (_remaining < _min)
                _min = _remaining;
        }
        log_debug("next delay: %" PRIu64, _min.value());
        return _min;
    };

    if (m_timer_processing and in_loop_thread()) {
        return _get_min_delay();
    } else {
        local_lock _lock(m_timers_mutex);
        return _get_min_delay();
    }
}

void
event_loop::process_timer ()
{
    m_timer_processing = true;
    {
        local_lock _lock(m_timers_mutex);
        for (auto _iter = m_timers.begin(); _iter != m_timers.end(); ++_iter) {
            if (m_timers.empty())
                break;
            timer_ptr _timer = *_iter;
            assert(_timer);
            auto _now_time = timestamp::monotonic();
            auto _remaining = _timer->m_timeout.remaining(_now_time);

            // remove closed timer
            if (_timer->closed())
                goto remove;

            // handle timer
            if (_remaining  > timestamp(KNGIN_TIMER_REMAINING_PRESISION))
                continue;

            log_excp_error(
                _timer->on_events(*this),
                "epoller_event_handler::on_events() error"
            );

            // remove once timer
            if (!_timer->closed() and !_timer->m_timeout.persist()) {
remove:
                _timer->close();
                _iter = m_timers.erase(_iter);
                continue;
            }

            // update
            _timer->m_timeout.update(_now_time);
        }
    }
    m_timer_processing = false;
}
#endif

void
event_loop::sort_events (epoll_event_set &_events, size_t _size)
{
    std::sort(_events.begin(), _events.begin() + _size,
        [] (struct ::epoll_event &_e1, struct ::epoll_event &_e2) -> bool {
        epoller_event *_ptr1 = static_cast<epoller_event *>(_e1.data.ptr);
        epoller_event *_ptr2 = static_cast<epoller_event *>(_e2.data.ptr);
        return (_ptr1->type() > _ptr2->type() or
                (_ptr1->type() == _ptr2->type() and
                 _ptr1->priority() > _ptr2->priority()));
    }); // end of operator < for sortting
}

void
event_loop::process_events (epoll_event_set &_events, size_t _size)
{
    for (uint32_t _i = 0; _i < _size; _i++) {
        auto *_ptr = static_cast<epoller_event *>(_events[_i].data.ptr);
        assert(_ptr);
        assert(_ptr->registed());
        if (_ptr->registed()) {
            log_excp_error(
                _ptr->on_events(*this, _events[_i].events),
                "epoller_event_handler::on_events() error"
            );
        }
    }
}

void
event_loop::wakeup ()
{
    if (m_waker and m_looping and m_waker->registed())
        m_waker->notify(); // blocked
}

void
event_loop::add_timer (timer_ptr &_timer)
{
#if (ON == KNGIN_USE_TIMERFD)
    register_event(_timer);
#else
    if (m_timer_processing and m_looping and in_loop_thread()) {
        m_timers.push_front(_timer);
    } else {
        local_lock _lock(m_timers_mutex);
        m_timers.push_front(_timer);
    }
    wakeup();
#endif
}

KNGIN_NAMESPACE_K_END

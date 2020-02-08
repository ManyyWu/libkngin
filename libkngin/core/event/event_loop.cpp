#ifdef _WIN32
#else
#include <unistd.h>
#include <sys/eventfd.h>
#endif
#include <cstring>
#include <algorithm>
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/base/thread.h"
#include "core/event/event_loop.h"
#include "core/event/event.h"
#include "core/event/epoller.h"

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
      m_events(KNGIN_RESERVED_EPOLLELR_EVENT),
      m_timers_mutex()
{
} catch (...) {
    log_fatal("event_loop::pimpl() error");
    throw;
}

event_loop::~event_loop () KNGIN_NOEXCP
{
    size_t _size = m_timers.size();
    if (_size)
    {
        log_warning("there are still have %" PRIu64
                    " uncancelled timer in epoller", _size);
        for (auto _iter : m_timers)
            cancel(_iter.second);
    }
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

            // wait for events
            uint32_t _size = m_epoller.wait(m_events, KNGIN_EPOLLER_TIMEOUT);
            if (m_stop)
                break;
            //log_warning("the epoller is awaken with %" PRIu64 " events", _size);

            // sort the events by priority and type(timer > event > file)
            std::sort(m_events.begin(), m_events.begin() + _size,
                [] (struct ::epoll_event &_e1, struct ::epoll_event &_e2) -> bool {
                epoller_event *_ptr1 = static_cast<epoller_event *>(_e1.data.ptr);
                epoller_event *_ptr2 = static_cast<epoller_event *>(_e2.data.ptr);
                return (_ptr1->type() > _ptr2->type() or
                        (_ptr1->type() == _ptr2->type() and
                         _ptr1->priority() > _ptr2->priority()));
            }); // end of operator < for sortting

            // process events
            for (uint32_t _i = 0; _i < _size; _i++) {
                auto *_ptr = static_cast<epoller_event *>(m_events[_i].data.ptr);
                assert(_ptr);
                assert(_ptr->registed());
                if (_ptr->registed()) {
                    log_excp_error(
                        _ptr->on_events(*this, m_events[_i].events),
                        "epoller_event_handler::on_events() error"
                    );
                }
            }
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
    assert(_timer->registed());
    {
        local_lock _lock(m_timers_mutex);
        assert(m_timers.find(_timer->key()) != m_timers.end());
        m_timers.erase(_timer->key());
        if (m_looping)
            remove_event(*_timer);
        _timer->close();
    }
}

void
event_loop::cancel (timer::timerid &_id)
{
    auto _timer = _id.weak_ptr().lock();
    assert(_timer);
    assert(_timer->registed());
    {
        local_lock _lock(m_timers_mutex);
        assert(m_timers.find(_id.key()) != m_timers.end());
        m_timers.erase(_timer->key());
        remove_event(*_timer);
        _timer->close();
    }
}

timer::timerid
event_loop::run_after (timestamp _delay, timeout_handler &&_handler,
                       bool _realtime /* = false */)
{
    auto _timer = std::make_shared<timer>(std::move(_handler), _realtime);
    _timer->set_time(_delay, 0, false);
    add_timer(_timer);
    return timerid(_timer);
}

timer::timerid
event_loop::run_every (timestamp _interval, timeout_handler &&_handler,
                       bool _realtime /* = false */)
{
    auto _timer = std::make_shared<timer>(std::move(_handler), _realtime);
    _timer->set_time(_interval, _interval, false);
    add_timer(_timer);
    return timerid(_timer);
}

timer::timerid
event_loop::run_at (timestamp _absval, timeout_handler &&_handler,
                    bool _realtime /* = false */)
{
    auto _timer = std::make_shared<timer>(std::move(_handler), _realtime);
    _timer->set_time(_absval, 0, true);
    add_timer(_timer);
    return timerid(_timer);
}

void
event_loop::process_events ()
{

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
    register_event(_timer);
    {
        local_lock _lock(m_timers_mutex);
        assert(m_timers.find(_timer->key()) == m_timers.end());
        m_timers.insert(std::pair<int, timer_ptr>(_timer->key(), _timer));
    }
}

KNGIN_NAMESPACE_K_END

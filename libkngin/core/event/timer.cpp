#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/event/timer.h"
#include "core/event/event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/event/timer.cpp"

KNGIN_NAMESPACE_K_BEGIN

#if (ON == KNGIN_USE_TIMERFD)

timer::timerid::timerid (timer_ptr _timer)
    : m_timer(_timer)
{
}

timer::timerid::timerid (const timerid &_timer)
    : m_timer(_timer.m_timer)
{
}

timer::timerid &
timer::timerid::operator = (const timerid &_timer)
{
    m_timer = _timer.m_timer;
    return *this;
}

timestamp
timer::timerid::interval () const KNGIN_NOEXCP
{
    if (auto _timer = m_timer.lock())
        return assert(_timer), _timer->m_interval;
    return 0;
}

bool
timer::timerid::abs () const KNGIN_NOEXCP
{
    if (auto _timer = m_timer.lock())
        return assert(_timer), _timer->m_abs;
    return false;
}

int
timer::timerid::key () const KNGIN_NOEXCP
{
    if (auto _timer = m_timer.lock())
        return assert(_timer), _timer->key();
    return INVALID_FD;
}

timer::timer (timeout_handler &&_handler)
try
    : epoller_event(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC),
                    epoller_event::EVENT_TYPE_TIMER),
      m_timeout_handler(std::move(_handler)),
      m_initval(0),
      m_interval(0),
      m_abs(false)
{
    arg_check(m_timeout_handler);
    if (invalid())
        throw k::system_error("::timerfd_create() erorr");
} catch (...) {
    log_fatal("timer::timer() error");
    throw;
}

timer::~timer () KNGIN_NOEXCP
{
    if (registed())
        log_warning("the timer must be closed"
                    " before object disconstructing");
    if (valid())
        ignore_excp(this->close());
}

/*
timestamp
timer::get_time ()
{
    assert(valid());

    itimerspec _its;
    if (timerfd_gettime(m_fd, &_its) < 0)
        throw k::system_error("timerfd_gettime() error");
    return _its.it_value;
}*/

void
timer::set_time (timestamp _val, timestamp _interval,
                 bool _abs /* = false */)
{
    assert(valid());
    m_initval = _val;
    m_interval = _interval;
    m_abs = _abs;

    itimerspec _its;
    _val.to_timespec(_its.it_value);
    _interval.to_timespec(_its.it_interval);
    if (timerfd_settime(m_fd, _abs ? TFD_TIMER_ABSTIME : 0,
                        &_its, nullptr) < 0)
        throw k::system_error("timerfd_settime() error");
    enable_read();
}

void
timer::close ()
{
    if (valid())
        filefd::close();
}

void
timer::on_events (event_loop &_loop, uint32_t _flags)
{
    try {
        if ((EPOLLHUP | EPOLLERR | EPOLLIN) & _flags)
            this->on_read(_loop);
    } catch (std::exception &_e) {
        log_fatal("caught an exception in timer::on_event(), %s", _e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in timer::on_event()");
        throw;
    }
}

void
timer::on_read (event_loop &_loop)
{
    assert(valid());
    auto _self = self();

    char _arr[8];
    in_buffer _buf(_arr, 8);
    log_excp_fatal(
        this->readn(_buf), // blocked
        "timer::readn() error"
    );

    if (m_timeout_handler) {
        log_excp_error(
            m_timeout_handler(_self),
            "timer::m_timeout_handler() error"
        );
    }

    if (!m_interval and registed())
        _loop.cancel(_self);
}

#else

timer::timer (timeout_handler &&_handler)
try
    : m_timeout_handler(std::move(_handler)),
      m_timeout(),
      m_closed(true)
{
    arg_check(m_timeout_handler);
} catch (...) {
    log_fatal("timer::timer() error");
    throw;
}

void
timer::set_time (timestamp _cur_time, timestamp _delay, bool _persist)
{
    m_timeout.update(_cur_time);
    m_timeout.set_interval(_delay);
    m_timeout.set_persist(_persist);
    m_closed = false;
}

void
timer::on_events (event_loop &_loop)
{
    auto _self = self();
    if (m_timeout_handler) {
        log_excp_error(
            m_timeout_handler(_self),
            "timer::m_timeout_handler() error"
        );
    }
}

#endif

KNGIN_NAMESPACE_K_END

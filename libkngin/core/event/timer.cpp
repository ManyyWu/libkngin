#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/event/timer.h"
#include "core/event/event_loop.h"

#define KNGIN_FILENAME "libkngin/core/event/timer.cpp"

KNGIN_NAMESPACE_K_BEGIN

#if (ON == KNGIN_USE_TIMERFD)

timer::timerid::timerid (timer_ptr timer)
    : m_timer(timer)
{
}

timer::timerid::timerid (const timerid &timer)
    : m_timer(timer.m_timer)
{
}

timer::timerid &
timer::timerid::operator = (const timerid &timer)
{
    m_timer = timer.m_timer;
    return *this;
}

timestamp
timer::timerid::interval () const noexcept
{
    if (auto timer = m_timer.lock())
        return assert(timer), timer->m_interval;
    return 0;
}

bool
timer::timerid::abs () const noexcept
{
    if (auto timer = m_timer.lock())
        return assert(timer), timer->m_abs;
    return false;
}

int
timer::timerid::key () const noexcept
{
    if (auto timer = m_timer.lock())
        return assert(timer), timer->key();
    return INVALID_FD;
}

timer::timer (timeout_handler &&handler)
try
    : epoller_event(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC),
                    epoller_event::EVENT_TYPE_TIMER),
      m_timeout_handler(std::move(handler)),
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

timer::~timer () noexcept
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

    itimerspec its;
    if (timerfd_gettime(m_fd, &its) < 0)
        throw k::system_error("timerfd_gettime() error");
    return its.it_value;
}*/

void
timer::set_time (timestamp val, timestamp interval,
                 bool abs /* = false */)
{
    assert(valid());
    m_initval = val;
    m_interval = interval;
    m_abs = abs;

    itimerspec its;
    val.to_timespec(its.it_value);
    interval.to_timespec(its.it_interval);
    if (timerfd_settime(m_fd, abs ? TFD_TIMER_ABSTIME : 0,
                        &its, nullptr) < 0)
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
timer::on_events (event_loop &loop, uint32_t flags)
{
    try {
        if ((EPOLLHUP | EPOLLERR | EPOLLIN) & flags)
            this->on_read(loop);
    } catch (std::exception &e) {
        log_fatal("caught an exception in timer::on_event(), %s", e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in timer::on_event()");
        throw;
    }
}

void
timer::on_read (event_loop &loop)
{
    assert(valid());
    auto s = self();

    char arr[8];
    in_buffer buf(arr, 8);
    log_excp_fatal(
        this->readn(buf), // blocked
        "timer::readn() error"
    );

    if (m_timeout_handler) {
        log_excp_error(
            m_timeout_handlers,
            "timer::m_timeout_handler() error"
        );
    }

    if (!m_interval and registed())
        loop.cancels;
}

#else

timer::timer (timeout_handler &&handler)
try
    : m_timeout_handler(std::move(handler)),
      m_timeout(),
      m_closed(true)
{
    arg_check(m_timeout_handler);
} catch (...) {
    log_fatal("timer::timer() error");
    throw;
}

void
timer::set_time (timestamp cur_time, timestamp delay, bool persist)
{
    m_timeout.update(cur_time);
    m_timeout.set_interval(delay);
    m_timeout.set_persist(persist);
    m_closed = false;
}

void
timer::on_events (event_loop &loop)
{
    auto s = self();
    if (m_timeout_handler) {
        log_excp_error(
            m_timeout_handler(s),
            "timer::m_timeout_handler() error"
        );
    }
}

#endif

KNGIN_NAMESPACE_K_END

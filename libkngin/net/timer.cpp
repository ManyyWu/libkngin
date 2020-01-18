#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include "core/common.h"
#include "core/system_error.h"
#include "net/timer.h"
#include "net/event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/timer.cpp"

KNGIN_NAMESPACE_K_BEGIN

timer::timerid::timerid (timer_ptr _timer)
    : m_timer(_timer),
      m_key(_timer->key()),
      m_initval(_timer->m_initval),
      m_interval(_timer->m_interval),
      m_realtime(_timer->m_realtime),
      m_abs(_timer->m_abs)
{
}

timer::timerid::timerid (const timerid &_timer)
    : m_timer(_timer.m_timer),
      m_key(_timer.m_key),
      m_initval(_timer.m_initval),
      m_interval(_timer.m_interval),
      m_realtime(_timer.m_realtime),
      m_abs(_timer.m_abs)
{
}

timer::timerid &
timer::timerid::operator = (const timerid &_timer)
{
    m_timer = _timer.m_timer;
    m_key = _timer.m_key;
    m_initval = _timer.m_initval;
    m_interval = _timer.m_interval;
    m_realtime = _timer.m_realtime;
    m_abs = _timer.m_abs;
    return *this;
}

timer::timer (timeout_handler &&_handler, bool _realtime /* = false */)
try
    : epoller_event(::timerfd_create(_realtime ? CLOCK_REALTIME : CLOCK_MONOTONIC,
                                     TFD_CLOEXEC),
                    epoller_event::EVENT_TYPE_TIMER),
      m_timeout_handler(std::move(_handler)),
      m_initval(0),
      m_interval(0),
      m_realtime(_realtime),
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

timestamp
timer::get_time ()
{
    assert(valid());

    itimerspec _its;
    if (timerfd_gettime(m_fd, &_its) < 0)
        throw k::system_error("timerfd_gettime() error");
    return _its.it_value;
}

void
timer::set_time (timestamp _val, timestamp _interval,
                 bool _abs /* = false */, bool _persist /* = false */)
{
    assert(valid());
    m_initval = _val;
    m_interval = _interval;
    m_abs = _abs;
    m_persist = _persist;

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

    if (!m_persist && registed())
        _loop.cancel(_self);
}

KNGIN_NAMESPACE_K_END

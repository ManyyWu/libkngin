#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include "core/common.h"
#include "core/system_error.h"
#include "net/timer.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/timer.cpp"

KNGIN_NAMESPACE_K_BEGIN

timer::timer (timeout_handler &&_handler, bool _abs /* = false */)
    try
    : epoller_event(::timerfd_create(_abs ? CLOCK_REALTIME : CLOCK_MONOTONIC, TFD_CLOEXEC),
                    epoller_event::EVENT_TYPE_TIMER),
      m_timeout_handler(std::move(_handler))
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
    assert(invalid());
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
timer::set_time (timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    assert(valid());

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
    assert(!registed());
    filefd::close();
}

void
timer::on_error ()
{
    on_read();
}

void
timer::on_read ()
{
    assert(valid());

    char _arr[8];
    in_buffer _buf(_arr, 8);
    log_excp_fatal(
        this->readn(_buf), // blocked
        "timer::readn() error"
    );

    if (m_timeout_handler) {
        log_excp_error(
            m_timeout_handler(),
            "timer::m_timeout_handler() error"
        );
    }
}

KNGIN_NAMESPACE_K_END

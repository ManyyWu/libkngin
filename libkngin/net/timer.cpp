#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include "core/common.h"
#include "core/system_error.h"
#include "net/timer.h"
#include "net/epoller_event.h"
#include "net/event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/timer.cpp"

KNGIN_NAMESPACE_K_BEGIN

timer::timer (event_loop_pimpl_ptr _loop,
              timeout_handler &&_timeout_handler)
    try
    : epoller_event(::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC)),
      m_loop(_loop),
      m_timeout_handler(std::move(_timeout_handler))
{
    arg_check(m_loop && _timeout_handler);
    if (FD_VALID(m_fd))
        throw k::system_error("::timerfd_create() erorr");
    set_closeexec(true);
} catch (...) {
    log_fatal("timer::timer() error");
    throw;
}

timer::~timer () KNGIN_NOEXCP
{
    if (is_single_ref_ptr(m_loop))
        return; // removed
    ignore_excp(
        if (m_loop->registed(self()))
            m_loop->remove_event(self());
        this->close();
    );

}

timestamp
timer::get_time ()
{
    itimerspec _its;
    if (timerfd_gettime(m_fd, &_its) < 0)
        throw k::system_error("timerfd_gettime() error");
    return _its.it_value;
}

void
timer::set_time (timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    itimerspec _its;
    _val.to_timespec(_its.it_value);
    _interval.to_timespec(_its.it_interval);
    if (timerfd_settime(m_fd, _abs ? TFD_TIMER_ABSTIME : 0,
                        &_its, nullptr) < 0)
        throw k::system_error("timerfd_settime() error");
    enable_read();
}

void
timer::on_error ()
{
    on_read();
}

void
timer::on_read ()
{
    char _arr[8];
    in_buffer _buf(_arr, 8);
    this->readn(_buf); // blocked

    if (m_timeout_handler)
        m_timeout_handler();
}

KNGIN_NAMESPACE_K_END

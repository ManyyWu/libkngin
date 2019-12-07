#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include <cstring>
#include "core/common.h"
#include "core/system_error.h"
#include "net/timer.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/timer.cpp"

KNGIN_NAMESPACE_K_BEGIN

timer::pimpl::pimpl (const event_loop_pimpl_ptr &_loop, 
                     const timeout_handler &&_timeout_handler)
    try
    : epoller_event(::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK)),
      m_loop(_loop),
      m_timeout_handler(_timeout_handler)
{
    arg_check(m_loop && _timeout_handler);
    if (FD_VALID(m_fd))
        throw k::system_error("::timerfd_create() erorr");
} catch (...) {
    log_fatal("timer::timer() error");
    throw;
}

timer::pimpl::~pimpl () KNGIN_NOEXP
{
}

timestamp
timer::pimpl::get_time ()
{
    itimerspec _its;
    std::error_code _ec = int2ec(timerfd_gettime(m_fd, &_its));
    if (_ec)
        throw k::system_error("timerfd_gettime() error");
    return _its.it_value;
}

void
timer::pimpl::set_time (timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    itimerspec _its;
    _val.to_timespec(_its.it_value);
    _interval.to_timespec(_its.it_interval);
    std::error_code _ec = int2ec(timerfd_settime(m_fd, _abs ? TFD_TIMER_ABSTIME : 0,
                                                 &_its, nullptr));
    if (_ec)
        throw k::system_error("timerfd_settime() error");
}

void
timer::pimpl::on_error()
{
    if (single_ref_ptr(m_loop))
        return;
    std::shared_ptr<timer::pimpl> _ptr = self();
    m_loop->remove_event(_ptr);
}

void
timer::pimpl::on_read ()
{
    char _arr[8];
    in_buffer _buf(_arr, 8);
    this->readn(_buf); // blocked

    if (m_timeout_handler)
        m_timeout_handler();
}

KNGIN_NAMESPACE_K_END

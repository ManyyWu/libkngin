#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include <cstring>
#include "net/timer.h"
#include "core/common.h"
#include "core/system_error.h"
#include "net/event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/timer.cpp"

KNGIN_NAMESPACE_K_BEGIN

timer::timer (event_loop_pimpl_ptr _loop)
    try
    : filefd(::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK)),
      m_loop(_loop),
      m_timeout_cb(nullptr),
      m_event(_loop, this),
      m_stopped(true)
{
    check(_loop);
    if (FD_VALID(m_fd))
        throw k::system_error("::timerfd_create() erorr");
} catch (...) {
    log_fatal("timer::timer() error");
    throw;
}

timer::~timer () KNGIN_NOEXP
{
    if (!m_stopped)
        ignore_exp(m_event.remove());
}

void
timer::start (timer_cb &&_timeout_cb, timestamp _val, timestamp _interval,
              bool _abs /* = false */)
{
    arg_check(_timeout_cb);
    check(m_stopped);

    m_timeout_cb = std::move(_timeout_cb);
    set_time(_val, _interval, _abs);
    m_event.set_read_cb(std::bind(&timer::on_timeout, this));
    m_event.start();
    m_stopped = false;
}

void
timer::stop ()
{
    check(!m_stopped);

    m_event.remove();
    m_stopped = true;
}

timestamp
timer::get_time ()
{
    check(!m_stopped);

    itimerspec _its;
    std::error_code _ec = int2ec(timerfd_gettime(m_fd, &_its));
    if (_ec)
        throw k::system_error("timerfd_gettime() error");
    return _its.it_value;
}

void
timer::set_time (timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    check(!m_stopped);

    itimerspec _its;
    _val.to_timespec(_its.it_value);
    _interval.to_timespec(_its.it_interval);
    std::error_code _ec = int2ec(timerfd_settime(m_fd, _abs ? TFD_TIMER_ABSTIME : 0,
                                                 &_its, nullptr));
    if (_ec)
        throw k::system_error("timerfd_settime() error");
}

void
timer::on_timeout () KNGIN_NOEXP
{
    check(!m_stopped);

    char _arr[8];
    in_buffer _buf(_arr, 8);
    std::error_code _ec;
    size_t _ret = this->readn(_buf, _ec); // blocked
//    if (m_timeout_cb)
//        ignore_exp(m_timeout_cb(_ec));
#warning "error_code"
}

KNGIN_NAMESPACE_K_END

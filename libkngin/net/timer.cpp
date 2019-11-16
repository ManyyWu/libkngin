#ifdef _WIN32
#else
#include <sys/timerfd.h>
#endif
#include <cstring>
#include "common.h"
#include "event_loop.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/timer.cpp"

__NAMESPACE_BEGIN

timer::timer (event_loop *_loop)
    : filefd(::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK)),
      m_loop(_loop),
      m_timeout_cb(nullptr),
      m_event(_loop, this),
      m_stopped(true)
{
    check(_loop);
    if (__fd_valid(m_fd)) {
        log_fatal("timerfd_create() error - %s:%d", strerror(errno), errno);
        throw exception("timer::timer() erorr");
    }
    //log_debug("new timer, fd = %d", m_fd);
}

timer::~timer ()
{
    if (!m_stopped)
        m_event.remove();
}

void
timer::start (timer_cb &&_timeout_cb, timestamp _val, timestamp _interval, bool _abs /* = false */)
{
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
    int _ret = timerfd_gettime(m_fd, &_its);
    if (_ret < 0)
        log_fatal("timerfd_gettime() error - %s:%d", strerror(errno), errno);
    return _its.it_value;
}

void
timer::set_time (timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    check(!m_stopped);
    itimerspec _its;
    _val.to_timespec(_its.it_value);
    _interval.to_timespec(_its.it_interval);

    int _ret = timerfd_settime(m_fd, _abs ? TFD_TIMER_ABSTIME : 0, &_its, nullptr);
    if (_ret < 0)
        log_fatal("timerfd_settime() error - %s:%d", strerror(errno), errno);
}

void
timer::on_timeout ()
{
    check(!m_stopped);
    buffer _val(8);
    ssize_t _ret = this->read(_val, 8); // blocked
    if (_ret < 0)
        log_error("timer::on_timeout() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("timer::on_timeout() error, read %" PRId64 " bytes instead of 8", _ret);
    else
        if (m_timeout_cb)
            m_timeout_cb();
}

__NAMESPACE_END

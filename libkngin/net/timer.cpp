#include <sys/timerfd.h>
#include "errno.h"
#include "define.h"
#include "common.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

timer::timer (event_loop *_loop)
    : filefd(::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK)),
      m_loop(_loop),
      m_event(_loop, this),
      m_stopped(false)
{
    check(_loop);
    if (__fd_valid(m_fd)) {
        log_fatal("timerfd_create() error - %s:%d", strerror(errno), errno);
        throw exception("timer::timer() erorr");
    }

    log_debug("new timer, fd = %d", m_fd);
}

timer::~timer ()
{
    if (!m_stopped)
        m_event.remove();
}

void
timer::start (timer_cb &&_timeout_cb, timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    check(!m_stopped);
    m_timeout_cb = std::move(_timeout_cb);
    set_time(_val, _interval, _abs);
    m_event.set_read_cb(std::bind(&timer::m_timeout_cb, this));
    m_event.start();
}

void
timer::stop ()
{
    check(!m_stopped);
    set_time(0, 0);
    m_event.remove();
    close();
    m_stopped = true;
}

timestamp
timer::get_time ()
{
    check(__fd_valid(m_fd));
    check(!m_stopped);

    itimerspec _its;
    int _ret = timerfd_gettime(m_fd, &_its);
    if (_ret < 0)
        log_fatal("timerfd_gettime() error - %s:%d", strerror(errno), errno);
    return _its.it_value;
}

epoller_event *
timer::get_event ()
{
    return &m_event;
}

void
timer::set_time (timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    check(__fd_valid(m_fd));
    check(!m_stopped);

    itimerspec _its;
    _val.to_timespec(_its.it_value);
    _interval.to_timespec(_its.it_interval);

    int _ret = timerfd_settime(m_fd, _abs ? TFD_TIMER_ABSTIME : 0, &_its, NULL);
    if (_ret < 0)
        log_fatal("timerfd_settime() error - %s:%d", strerror(errno), errno);
}

__NAMESPACE_END

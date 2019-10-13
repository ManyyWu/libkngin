#include <sys/timerfd.h>
#include <cstring>
#include "define.h"
#include "common.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

timer::timer (event_loop *_loop)
    : filefd(::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK)),
      m_loop(_loop),
      m_event(_loop, this)
{
    kassert(_loop);
    if_not (__fd_valid(m_fd)) {
        log_fatal("timerfd_create() error - %s:%d", strerror(errno), errno);
        throw exception("timer::timer() erorr");
    }
}

timer::~timer ()
{
}

void
timer::start (timer_cb &&_timeout_cb, timestamp _val, timestamp _interval, bool _abs /* = false */)
{
    m_timeout_cb = std::move(_timeout_cb);
    set_time(_val, _interval, _abs);
    m_event.set_read_cb(std::bind(&timer::m_timeout_cb, this));
    m_loop->add_event(&m_event);
}

void
timer::stop ()
{
    set_time(0, 0);
    m_event.disable_read();
    m_loop->remove_event(&m_event);
}

timestamp
timer::get_time ()
{
    kassert(__fd_valid(m_fd));

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
    kassert(__fd_valid(m_fd));

    itimerspec _its;
    _val.to_timespec(_its.it_value);
    _interval.to_timespec(_its.it_interval);

    int _ret = timerfd_settime(m_fd, _abs ? TFD_TIMER_ABSTIME : 0, &_its, NULL);
    if (_ret < 0)
        log_fatal("timerfd_settime() error - %s:%d", strerror(errno), errno);
}

__NAMESPACE_END

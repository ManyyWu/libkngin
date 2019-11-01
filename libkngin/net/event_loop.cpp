#include <sys/eventfd.h>
#include <functional>
#include <unistd.h>
#include "define.h"
#include "errno.h"
#include "event_loop.h"
#include "local_lock.h"
#include "thread.h"
#include "epoller.h"
#include "filefd.h"

__NAMESPACE_BEGIN

event_loop::event_loop (thread *_thr)
    try
    : m_waker(this),
      m_thr(_thr),
      m_mutex(),
      m_epoller(this),
      m_looping(false),
      m_stop(false),
      m_fnq(),
      m_events(RESERVED_EPOLLELR_EVENT)
{
    check(__fd_valid(m_waker.fd()));
    check(_thr);

    m_waker.set_read_cb(std::bind(&event_loop::handle_wakeup, this));
    m_waker.set_nonblock(true);
    m_waker.set_closeexec(true);
    m_waker.start();
} catch (...) {
    log_fatal("event_loop::event_loop() error");
    throw;
}

event_loop::~event_loop ()
{
    m_waker.stop();
}

int
event_loop::loop ()
{
    check_thread();
    m_looping = true;

    try {
        while (!m_stop) {
            // wait for events
            uint32_t _size = m_epoller.wait(m_events, EPOLLER_TIMEOUT);
            log_debug("the epoller in thread \"%s\" is awardkened with %" PRIu64 " events",
                      m_thr->name(), _size);

            // process events
            for (uint32_t i = 0; i < _size; i++)
                ((epoller_event *)(m_events[i].data.ptr))->handle_events(m_events[i].events);

            // process queued events
            std::deque<queued_fn> _fnq;
            {
                local_lock _lock(m_mutex);
                _fnq.swap(m_fnq);
            }
            for (auto _iter : _fnq)
                _iter();
        }
    } catch (...) {
        m_looping = false;
        throw;
    }

    m_looping = false;
    return 0;
}

void
event_loop::stop ()
{
    m_stop = true;
    if (!in_loop_thread())
        wakeup();
}

bool
event_loop::looping ()
{
    return m_looping;
}

void
event_loop::check_thread ()
{
    check(m_thr->equal_to(thread::self()));
}

bool
event_loop::in_loop_thread()
{
    return (m_thr->equal_to(thread::self()));
}

bool
event_loop::add_event (epoller_event *_e)
{
    check(_e);
    bool _ret = m_epoller.register_event(_e);
    if (!_ret)
        log_fatal("epoller::register_event() erorr");
    if (!in_loop_thread())
        wakeup();
    return _ret;
}

bool
event_loop::remove_event (epoller_event *_e)
{
    check(_e);
    bool _ret = m_epoller.remove_event(_e);
    if (!_ret)
        log_fatal("epoller::remove_event() erorr");
    if (!in_loop_thread())
        wakeup();
    return _ret;
}

bool
event_loop::update_event (epoller_event *_e)
{
    check(_e);
    bool _ret = m_epoller.modify_event(_e);
    if (!_ret)
        log_fatal("epoller::modify_event() erorr");
    if (!in_loop_thread())
        wakeup();
    return _ret;
}

void
event_loop::run_in_loop (event_loop::queued_fn &&_fn)
{
    {
        local_lock _lock(m_mutex);
        m_fnq.push_back(_fn);
    }
    
    if (!in_loop_thread())
        wakeup();
}

void
event_loop::wakeup ()
{
    check(m_looping);

    buffer _val(8);
    _val.write_uint64(1);
    ssize_t _ret = m_waker.write(_val, sizeof(_val));
    if (_ret < 0)
        log_error("event_loop::wakeup() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("event_loop::wakeup() error, write %" PRId64 " bytes to waker instead of 8", _ret);
}

void
event_loop::handle_wakeup ()
{
    check(m_looping);

    buffer _val(8);
    ssize_t _ret = m_waker.read(_val, sizeof(_val));
    if (_ret < 0)
        log_error("event_loop::handle_wakeup() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("event_loop::handle_wakeup() error, read %" PRId64 " bytes from waker instead of 8", _ret);
}

__NAMESPACE_END

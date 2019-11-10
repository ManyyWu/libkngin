#include <sys/eventfd.h>
#include <functional>
#include <unistd.h>
#include "define.h"
#include "common.h"
#include "errno.h"
#include "event_loop.h"
#include "lock.h"
#include "thread.h"
#include "epoller.h"
#include "filefd.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/event_loop.cpp"

__NAMESPACE_BEGIN

event_loop::event_loop (thread *_thr)
    try
    : m_thr(_thr),
      m_mutex(),
      m_epoller(this),
      m_looping(false),
      m_stop(false),
      m_fnq(),
      m_events(RESERVED_EPOLLELR_EVENT),
      m_waker(this)
{
    check(__fd_valid(m_waker.fd()));
    check(_thr);
} catch (...) {
    log_fatal("event_loop::event_loop() error");
    throw;
}

event_loop::~event_loop ()
{
    log_debug("wait for loop in thread \"%s\" to end", m_thr->name());
    if (m_looping) {
        stop();
        // FIXME: wait for loop to end
    }
    
    log_debug("loop in thread \"%s\" closed", m_thr->name());
}

int
event_loop::loop (loop_started_cb &&_start_cb, loop_stopped_cb &&_stop_cb)
{
    check_thread();
    m_looping = true;

    try {
        m_waker.set_nonblock(false);
        m_waker.set_closeexec(true);
        m_waker.start(nullptr);
        if (_start_cb)
            _start_cb();

        while (!m_stop) {
            // wait for events
            uint32_t _size = m_epoller.wait(m_events, EPOLLER_TIMEOUT);
            if (m_stop)
                break;
            log_debug("the epoller in thread \"%s\" is awardkened with %" PRIu64 " events",
                      m_thr->name(), _size);

            // process events
            for (uint32_t i = 0; i < _size; ++i)
                ((epoller_event *)(m_events[i].data.ptr))->on_events(m_events[i].events);

            // process queued events
            std::deque<queued_fn> _fnq;
            if (!m_fnq.empty())
            {
                local_lock _lock(m_mutex);
                _fnq.swap(m_fnq);
            }
            for (auto _iter : _fnq)
                _iter();
            log_debug("the epoller in thread \"%s\" handled %" PRIu64 " task",
                      m_thr->name(), _fnq.size());
        }
    } catch (...) {
        if (_stop_cb)
            _stop_cb();
        m_waker.stop();
        m_looping = false;
        log_fatal("caught an exception in event_loop of thread \"%s\"", m_thr->name());
        throw;
    }

    if (_stop_cb)
        _stop_cb();
    m_waker.stop();
    m_looping = false;
    log_info("event_loop in thread \"%s\" is stopped", m_thr->name());
    return 0;
}

void
event_loop::stop ()
{
    check(m_looping);
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
    //log_debug("m_thr:%" PRIu64 ", self:%" PRIu64, m_thr->get_interface(), thread::self());
    return (m_thr->equal_to(thread::self()));
}

bool
event_loop::add_event (epoller_event *_e)
{
    check(_e);
    check(m_looping);

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
    check(m_looping);

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
    check(m_looping);

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
    check(m_looping);

    {
        local_lock _lock(m_mutex);
        m_fnq.push_back(std::move(_fn));
    }
    
    if (!in_loop_thread())
        wakeup();
}

void
event_loop::wakeup ()
{
    check(m_looping);
    log_debug("wakeup event_loop in thread \"%s\"", m_thr->name());
    if (m_waker.stopped())
        return;

    buffer _val(8);
    _val.write_uint64(1);
    ssize_t _ret = m_waker.write(_val, 8); // blocked
    if (_ret < 0)
        log_error("event_loop::wakeup() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("event_loop::wakeup() error, write %" PRId64 " bytes to waker instead of 8", _ret);
}

__NAMESPACE_END

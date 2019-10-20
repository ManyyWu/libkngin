#include <sys/eventfd.h>
#include <functional>
#include <cstring>
#include <unistd.h>
#include "define.h"
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
      m_stop(false)
{
    kassert(__fd_valid(m_waker.fd()));
    kassert(_thr);

    // listen waker
    m_waker.set_read_cb(std::bind(&event_loop::handle_wakeup, this));
    m_epoller.register_event(m_waker.get_event());
} catch (...) {
    log_fatal("event_loop::event_loop() error");
    throw;
}

event_loop::~event_loop ()
{
    // m_waker_event.disable_read();
}

int
event_loop::loop (void *_args)
{
    check_thread();
    m_looping = true;

    epoller::event_list _events;
    while (!m_stop) {
        _events.clear();

        // wait for events
        m_epoller.wait(EPOLLER_TIMEOUT, _events);
        log_debug("the epoller in thread \"%s\" is awardkened with %ul events",
                  m_thr->name(), _events.size());

        // process events
        for (auto _iter : _events)
            _iter->handle_events();
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
    kassert(m_thr->equal_to(thread::self()));
}

bool
event_loop::in_loop_thread()
{
    return (m_thr->equal_to(thread::self()));
}

bool
event_loop::add_event (epoller_event *_e)
{
    kassert(_e);
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
    kassert(_e);
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
    kassert(_e);
    bool _ret = m_epoller.modify_event(_e);
    if (!_ret)
        log_fatal("epoller::modify_event() erorr");
    if (!in_loop_thread())
        wakeup();
    return _ret;
}

void
event_loop::wakeup ()
{
    kassert(m_looping);

    buffer _val(8);
    _val.write_uint64(1);
    ssize_t _ret = m_waker.write(_val, sizeof(_val));
    if (_ret < 0)
        log_error("event_loop::wakeup() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("event_loop::wakeup() error, write %d bytes to waker instead of 8", _ret);
}

void
event_loop::handle_wakeup ()
{
    buffer _val(8);
    ssize_t _ret = m_waker.read(_val, sizeof(_val));
    if (_ret < 0)
        log_error("event_loop::handle_wakeup() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("event_loop::handle_wakeup() error, read %d bytes from waker instead of 8", _ret);
}

__NAMESPACE_END

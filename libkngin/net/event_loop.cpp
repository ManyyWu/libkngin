#include <sys/eventfd.h>
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
    : m_waker(),
      m_waker_event(this, &m_waker),
      m_thr(_thr),
      m_mutex(),
      m_epoller(this)
{
    kassert(__fd_valid(m_waker.fd()));
    kassert(_thr);

    // listen waker
    m_waker_event.set_read_cb(std::bind(&event_loop::handle_wakeup, this));
    m_epoller.register_event(&m_waker_event);
} catch (...) {
    log_fatal("event_loop::event_loop() error");
    throw;
}

event_loop::~event_loop ()
{
    m_waker_event.disable_read();
}

void
event_loop::update_event (epoller_event *_e)
{
    m_epoller.modify_event(_e);
}

int
event_loop::loop (void *_args)
{
    return 0;
}

void
event_loop::wakeup ()
{
    uint64_t _val = 1;
    ssize_t _ret = m_waker.write(&_val, sizeof(_val));
    if (_ret < 0)
        log_error("event_loop::wakeup() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("event_loop::wakeup() error, write %d bytes to waker instead of 8", _ret);
}

void
event_loop::handle_wakeup ()
{
    uint64_t _val = 1;
    ssize_t _ret = m_waker.read(&_val, sizeof(_val));
    if (_ret < 0)
        log_error("event_loop::handle_wakeup() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("event_loop::handle_wakeup() error, read %d bytes from waker instead of 8", _ret);
}

__NAMESPACE_END

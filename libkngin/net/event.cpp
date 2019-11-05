#include <sys/eventfd.h>
#include "define.h"
#include "error.h"
#include "event.h"
#include "filefd.h"
#include "common.h"
#include "event_loop.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/event.cpp"

__NAMESPACE_BEGIN

event::event (event_loop *_loop)
    : filefd(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
      m_loop(_loop),
      m_event(_loop, this),
      m_read_cb(nullptr),
      m_write_cb(nullptr),
      m_stopped(true)
{
    check(_loop);
    if (__fd_invalid(m_fd)) {
        log_fatal("eventfd() error - %s:%d", strerror(errno), errno);
        throw exception("event::event() erorr");
    }

    //log_debug("new event, fd = %d", m_fd);
}

event::~event()
{
    if (!m_stopped)
        m_event.remove();
}

void
event::start ()
{
    check(m_stopped);
    m_event.start();
    m_stopped = false;
}

void
event::update ()
{
    check(!m_stopped);
    m_event.update();
}

void
event::stop ()
{
    check(!m_stopped);
    m_stopped = true;
    m_event.remove();
}

void
event::set_read_cb (event_cb &&_cb)
{
    m_read_cb = std::move(_cb);
    m_event.set_read_cb(std::bind(&event::handle_read, this));
}

void
event::set_write_cb (event_cb &&_cb)
{
    m_write_cb = std::move(_cb);
    m_event.set_read_cb(std::bind(&event::handle_write, this));
}

epoller_event *
event::get_event ()
{
    return &m_event;
}

bool
event::stopped ()
{
    return m_stopped;
}

void
event::handle_read  ()
{
    check(!m_stopped);
    if (m_read_cb)
        m_read_cb(std::ref(*this));
}

void
event::handle_write ()
{
    check(!m_stopped);
    if (m_write_cb)
        m_write_cb(std::ref(*this));
}

__NAMESPACE_END

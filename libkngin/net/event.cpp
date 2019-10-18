#include <sys/eventfd.h>
#include <cstring>
#include "define.h"
#include "event.h"
#include "filefd.h"
#include "common.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

event::event (event_loop *_loop)
    : filefd(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
      m_loop(_loop),
      m_event(_loop, this)
{
    kassert(_loop);
    if (__fd_invalid(m_fd)) {
        log_fatal("eventfd() error - %s:%d", strerror(errno), errno);
        throw exception("event::event() erorr");
    }
    m_event.enable_once();
}

event::~event()
{
}

void
event::set_read_cb (event_cb &&_cb)
{
    m_event.set_read_cb(std::bind(&event::m_read_cb, this));
    m_loop->add_event(&m_event);
}

void
event::set_write_cb (event_cb &&_cb)
{
    m_event.set_read_cb(std::bind(&event::m_read_cb, this));
    m_loop->add_event(&m_event);
}

epoller_event *
event::get_event ()
{
    return &m_event;
}

__NAMESPACE_END

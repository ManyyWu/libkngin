#ifdef _WIN32
#else
#include <sys/eventfd.h>
#endif
#include <cstring>
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
      m_event_cb(nullptr),
      m_event(_loop, this),
      m_stopped(true)
{
    check(_loop);
    if (__fd_invalid(m_fd)) {
        log_fatal("::eventfd() error - %s:%d", strerror(errno), errno);
        throw k::exception("event::event() erorr");
    }
    //log_debug("new event, fd = %d", m_fd);
}

event::~event()
{
    if (!m_stopped)
        m_event.remove();
}

void
event::start (event_cb &&_cb)
{
    check(m_stopped);
    m_event_cb = std::move(_cb);
    m_event.set_read_cb(std::bind(&event::on_event, this));
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
event::notify ()
{
    check(!m_stopped);
    buffer _val(8);
    _val.write_uint64(1);
    ssize_t _ret = this->write(_val, 8); // blocked
    if (_ret < 0)
        log_error("filefd::write() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("filefd::wakeup() error, written %" PRId64 " bytes instead of 8", _ret);
}

void
event::stop ()
{
    check(!m_stopped);
    m_event.remove();
    m_stopped = true;
}

void
event::on_event ()
{
    check(!m_stopped);
    buffer _val(8);
    ssize_t _ret = this->read(_val, 8); // blocked
    if (_ret < 0)
        log_error("filefd::read() error - %s:%d", strerror(errno), errno);
    else if (_ret != sizeof(_ret))
        log_error("filefd::read() error, readed %" PRId64 " bytes instead of 8", _ret);
    else
        if (m_event_cb)
            m_event_cb();
}

__NAMESPACE_END

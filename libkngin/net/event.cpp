#ifdef _WIN32
#else
#include <sys/eventfd.h>
#endif
#include <cstring>
#include "core/common.h"
#include "core/system_error.h"
#include "net/event.h"
#include "net/filefd.h"
#include "net/event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/event.cpp"

KNGIN_NAMESPACE_K_BEGIN

event::event (event_loop_pimpl_ptr _loop) KNGIN_EXP
    try
    : filefd(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
      m_loop(std::move(_loop)),
      m_event_cb(nullptr),
      m_event(m_loop, this),
      m_stopped(true)
{
    arg_check(m_loop);
    if (FD_INVALID(m_fd))
        throw k::system_error("event::event() erorr");
} catch (...) {
    log_fatal("event::event() error");
    throw;
}

event::~event() KNGIN_NOEXP
{
    if (!m_stopped)
        ignore_exp(m_event.remove());
}

void
event::start (event_cb &&_cb) KNGIN_EXP
{
    arg_check(_cb);
    check(m_stopped);

    m_event_cb = std::move(_cb);
    m_event.set_read_cb(std::bind(&event::on_event, this));
    m_event.start();
    m_stopped = false;
}

void
event::update () KNGIN_EXP
{
    check(!m_stopped);

    m_event.update();
}

void
event::notify () KNGIN_EXP
{
    check(!m_stopped);

    buffer _val(8);
    _val.write_uint64(1);
    ssize_t _ret = this->write(_val, 8); // blocked
    if (_ret < 0)
        throw k::system_error("filefd::write() error");
}

void
event::stop () KNGIN_EXP
{
    check(!m_stopped);

    m_event.remove();
    m_stopped = true;
}

void
event::on_event () KNGIN_NOEXP
{
    check(!m_stopped);

    std::error_code _ec;
    buffer _val(8);
    size_t _ret = this->read(_val, 8, _ec); // blocked
//    if (m_event_cb)
//        ignore_exp(m_event_cb(_ec));
#warning "error_code"
}

KNGIN_NAMESPACE_K_END

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

event::event (event_loop_pimpl_ptr _loop)
    try
    : filefd(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
      m_loop(std::move(_loop)),
      m_event_handler(nullptr),
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
event::start (event_handler &&_handler)
{
    check(m_stopped);

    m_event_handler = std::move(_handler);
    m_event.set_read_handler(std::bind(&event::on_event, this));
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

    char _arr[8];
    in_buffer(_arr, 8).write_uint64(1);
    std::error_code _ec;
    size_t _ret = this->writen(out_buffer(_arr, 8), _ec); // blocked
    if (_ec)
        throw k::system_error("filefd::write() error");
}

void
event::notify (std::error_code &_ec) KNGIN_NOEXP
{
    check(!m_stopped);

    char _arr[8];
    out_buffer(_arr, 8);
    in_buffer(_arr, 8).write_uint64(1);
    this->writen(out_buffer(_arr, 8), _ec); // blocked
}

void
event::stop ()
{
    check(!m_stopped);

    m_event.remove();
    m_stopped = true;
}

void
event::on_event () KNGIN_NOEXP
{
    check(!m_stopped);

    char _arr[8];
    in_buffer _buf(_arr, 8);
    std::error_code _ec;
    size_t _ret = this->readn(_buf, _ec); // blocked
//    if (m_event_handler)
//        ignore_exp(m_event_hadnelr(_ec));
#warning "error_code"
}

KNGIN_NAMESPACE_K_END

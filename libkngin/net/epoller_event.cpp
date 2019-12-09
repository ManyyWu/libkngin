#include "core/common.h"
#include "core/exception.h"
#include "core/system_error.h"
#include "net/epoller_event.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/epoller_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

epoller_event::epoller_event (epollfd _fd) KNGIN_NOEXCP
    : filefd(_fd),
      m_flags(EPOLLHUP | EPOLLERR),
      m_event({0, nullptr})
{
}

epoller_event::epoller_event (epoller_event &&_e) KNGIN_NOEXCP
    : filefd(std::move(_e)),
      m_flags(_e.m_flags),
      m_event(_e.m_event)
{
    _e.m_event = {0, nullptr};
}

void
epoller_event::on_events (epoller_event *_ptr, uint32_t _events)
{
    assert(_ptr);
    try {
        if (EPOLLHUP & _events) { // RST
            _ptr->on_error();
            return;
        }
        if ((EPOLLERR & _events))
            _ptr->on_error();
        if ((EPOLLIN & _events))
            _ptr->on_read();
        if ((EPOLLOUT & _events))
            _ptr->on_write();
        if ((EPOLLPRI & _events))
            _ptr->on_oob();
    } catch (std::exception &_e) {
        log_fatal("caught an exception in epoller_event::on_event(), %s", _e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in epoller_event::on_event()");
        throw;
    }
}

KNGIN_NAMESPACE_K_END

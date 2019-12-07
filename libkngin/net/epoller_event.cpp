#include "core/common.h"
#include "core/exception.h"
#include "core/system_error.h"
#include "net/epoller_event.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/epoller_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

epoller_event::epoller_event (int _fd)
    : m_fd(_fd),
      m_flags(EPOLLHUP | EPOLLERR),
      m_event({0, nullptr}),
      m_in_handler(nullptr),
      m_out_handler(nullptr),
      m_err_handler(nullptr),
      m_pri_handler(nullptr)
{
}

void
epoller_event::on_events (uint32_t _events)
{
    try {
        if (EPOLLHUP & _events) // RST
        {
            if (m_err_handler)
                m_err_handler();
            else
                log_warning("unhandled event: EPOLLHUP");
            return;
        }
        if ((EPOLLERR & _events) && m_err_handler)
            m_err_handler();
        if ((EPOLLIN & _events) && m_in_handler)
            m_in_handler();
        if ((EPOLLOUT & _events) && m_out_handler)
            m_out_handler();
        if ((EPOLLPRI & _events) && m_pri_handler)
            m_pri_handler();
    } catch (std::exception &_e) {
        log_fatal("caught an exception in epoller_event::on_event(), %s", _e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in epoller_event::on_event()");
        throw;
    }
}

KNGIN_NAMESPACE_K_END

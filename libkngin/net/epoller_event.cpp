#include "core/common.h"
#include "core/exception.h"
#include "core/system_error.h"
#include "net/event_loop.h"
#include "net/epoller_event.h"
#include "net/filefd.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/epoller_event.cpp"

KNGIN_NAMESPACE_K_BEGIN

epoller_event::epoller_event (event_loop_pimpl_ptr _loop, filefd *_s)
    try
    : m_loop(_loop),
      m_filefd(_s),
      m_flags(EPOLLHUP | EPOLLERR),
      m_in_handler(nullptr),
      m_out_handler(nullptr),
      m_err_handler(nullptr),
      m_pri_handler(nullptr),
      m_event({0, nullptr}),
      m_registed(false)
{
    arg_check(m_loop && _s);
} catch (...) {
    log_fatal("epoller_event::epoller_event() error");
    throw;
}

epoller_event::~epoller_event () KNGIN_NOEXP
{
    if (m_registed)
        ignore_exp(this->remove());
}

void
epoller_event::start ()
{
    m_loop->add_event(*this);
    m_registed = true;
}

void
epoller_event::update ()
{
    check(m_registed);
    m_loop->update_event(*this);
}

void
epoller_event::stop ()
{
    check(m_registed);
    disable_all();
    m_loop->update_event(*this);
}

void
epoller_event::remove ()
{
    check(m_registed);
    m_loop->remove_event(*this);
    m_registed = false;
}

void
epoller_event::on_events (uint32_t _flags)
{
    check(m_registed);

    try {
        if (EPOLLHUP & _flags) // RST
        {
            log_warning("event POLLHUP happend in fd %d", m_filefd->fd());
            if (m_err_handler)
                m_err_handler();
            else
                m_filefd->close();
            return;
        }
        if ((EPOLLERR & _flags) && m_err_handler)
            m_err_handler();
        if ((EPOLLIN & _flags) && m_in_handler)
            m_in_handler();
        if ((EPOLLOUT & _flags) && m_out_handler)
            m_out_handler();
        if ((EPOLLPRI & _flags) && m_pri_handler)
            m_pri_handler();
    } catch (std::exception &_e) {
        log_fatal("caught an exception in epoller_event::on_event() \"%s\"", _e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in epoller_event::on_event()");
        throw;
    }
}

KNGIN_NAMESPACE_K_END

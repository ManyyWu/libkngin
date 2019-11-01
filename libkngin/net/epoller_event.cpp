#include "define.h"
#include "epoller_event.h"
#include "event_loop.h"
#include "logfile.h"
#include "common.h"
#include "filefd.h"

__NAMESPACE_BEGIN

epoller_event::epoller_event(event_loop *_loop, filefd *_s)
    : m_loop(_loop), m_filefd(_s), m_flags(0),
      m_incb(nullptr), m_outcb(nullptr), m_errcb(nullptr),
      m_pricb(nullptr), m_closecb(nullptr)
{
    check(_loop && _s);
}

void
epoller_event::start ()
{
    m_loop->add_event(this);
}

void
epoller_event::update ()
{
    m_loop->update_event(this);
}

void
epoller_event::stop ()
{
    disable_all();
    m_loop->update_event(this);
}

void
epoller_event::remove ()
{
    m_loop->remove_event(this);
}

void
epoller_event::handle_events (uint32_t _flags)
{
//    log_debug("epoller_event::handle_events() flags = %d", m_flags);

    if (EPOLLHUP & _flags) // RST
    {
        m_filefd->close();
        log_warning("event POLLHUP happend in fd %d", m_filefd->fd());
        if (m_closecb)
            m_closecb();
        return;
    }
    if ((EPOLLERR & _flags) && m_errcb)
        m_errcb();
    if ((EPOLLIN & _flags) && m_incb)
        m_incb();
    if ((EPOLLOUT & _flags) && m_outcb)
        m_outcb();
    if ((EPOLLPRI & _flags) && m_pricb)
        m_pricb();
}

__NAMESPACE_END

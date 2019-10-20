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
    kassert(_loop && _s);
}

void
epoller_event::update ()
{
    kassert(m_loop);
    m_loop->update_event(this);
}

void
epoller_event::handle_events ()
{
    log_debug("epoller_event::handle_events() flags = %d", m_flags);

    if (EPOLLHUP & m_flags) // RST
    {
        log_warning("event POLLHUP happend in fd %d", m_filefd->fd());
        m_closecb(m_filefd);
        return;
    }
    if (EPOLLERR & m_flags && m_errcb)
        m_errcb(m_filefd);
    if (EPOLLIN & m_flags && m_incb)
        m_incb(m_filefd);
    if (EPOLLOUT & m_flags && m_outcb)
        m_outcb(m_filefd);
    if (EPOLLPRI & m_flags && m_pricb)
        m_pricb(m_filefd);
}

__NAMESPACE_END

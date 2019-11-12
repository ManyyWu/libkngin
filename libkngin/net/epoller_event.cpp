#include "define.h"
#include "epoller_event.h"
#include "event_loop.h"
#include "logfile.h"
#include "common.h"
#include "filefd.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/epoller_event.cpp"

__NAMESPACE_BEGIN

epoller_event::epoller_event(event_loop *_loop, filefd *_s)
    : m_loop(_loop), m_filefd(_s), m_flags(EPOLLHUP | EPOLLERR),
      m_incb(nullptr), m_outcb(nullptr), m_errcb(nullptr),
      m_pricb(nullptr), m_closecb(nullptr), m_event({0, nullptr}),
      m_registed(false)
{
    check(_loop && _s);
}

epoller_event::~epoller_event ()
{
    if (m_registed)
        this->remove();
}

void
epoller_event::start ()
{
    m_loop->add_event(this);
    m_registed = true;
}

void
epoller_event::update ()
{
    check(m_registed);
    m_loop->update_event(this);
}

void
epoller_event::stop ()
{
    check(m_registed);
    disable_all();
    m_loop->update_event(this);
}

void
epoller_event::remove ()
{
    check(m_registed);
    m_loop->remove_event(this);
    m_registed = false;
}

void
epoller_event::handle_events (uint32_t _flags)
{
    log_debug("epoller_event::handle_events() fd = %d, flags = %d", m_filefd->fd(), m_flags);

    check(m_registed);
    if (EPOLLHUP & _flags) // RST
    {
        log_warning("event POLLHUP happend in fd %d", m_filefd->fd());
        if (m_closecb)
            m_closecb();
        else
            m_filefd->close();
        return;
    }
    if ((EPOLLERR & _flags) && m_errcb)
        m_errcb();
    //else
    //    log_debug("unhandled event EPOLLERR");
    if ((EPOLLIN & _flags) && m_incb)
        m_incb();
    //else
    //    log_debug("unhandled event EPOLLIN");
    if ((EPOLLOUT & _flags) && m_outcb)
        m_outcb();
    //else
    //    log_debug("unhandled event EPOLLOUT");
    if ((EPOLLPRI & _flags) && m_pricb)
        m_pricb();
    //else
    //    log_debug("unhandled event EPOLLPRI");
}

__NAMESPACE_END

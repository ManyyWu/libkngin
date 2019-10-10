#include <sys/epoll.h>
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

epoller_event::~epoller_event ()
{
}

void
epoller_event::set_flags (int flags)
{
    m_flags = flags;
}

int
epoller_event::flags () const
{
    return m_flags;
}

void
epoller_event::enable_once ()
{
    m_flags = m_flags & ~EPOLLONESHOT;
}

void
epoller_event::disable_read ()
{
    m_flags = m_flags & ~EPOLLIN;
    m_incb = nullptr;
}

void
epoller_event::disable_write ()
{
    m_flags = m_flags & ~EPOLLOUT;
    m_outcb = nullptr;
}

void
epoller_event::disable_error ()
{
    m_errcb = nullptr;
}

void
epoller_event::disable_ergent ()
{
    m_flags = m_flags & ~EPOLLPRI;
    m_pricb = nullptr;
}

void
epoller_event::disable_once ()
{
    m_flags = m_flags & ~EPOLLONESHOT;
}

void
epoller_event::disable_close ()
{
    m_closecb = nullptr;
}

bool
epoller_event::pollin  () const
{
    return (m_flags & EPOLLIN);
}

bool
epoller_event::pollout () const
{
    return (m_flags & EPOLLOUT);
}

bool
epoller_event::pollpri () const
{
    return (m_flags & EPOLLPRI);
}

bool
epoller_event::pollonce () const
{
    return (m_flags & EPOLLONESHOT);
}

bool
epoller_event::pollhup () const
{
    return (m_flags & EPOLLHUP);
}

void
epoller_event::update ()
{
    kassert(m_loop);
    m_loop->update_event(this);
}

void
epoller_event::set_read_cb (epoller_event_cb _fn)
{
    m_incb = std::move(_fn);
    m_flags |= EPOLLIN;
}

void
epoller_event::set_write_cb (epoller_event_cb _fn)
{
    m_outcb = std::move(_fn);
    m_flags |= EPOLLOUT;
}

void
epoller_event::set_error_cb (epoller_event_cb _fn)
{
    m_errcb = std::move(_fn);
    m_flags |= EPOLLERR;
}

void
epoller_event::set_ergent_cb (epoller_event_cb _fn)
{
    m_pricb = std::move(_fn);
    m_flags |= EPOLLPRI;
}

void
epoller_event::set_close_cb (epoller_event_cb _fn)
{
    m_closecb = std::move(_fn);
    m_flags |= EPOLLHUP;
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

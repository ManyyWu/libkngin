#include <sys/epoll.h>
#include "define.h"
#include "epoller_event.h"
#include "event_loop.h"
#include "logfile.h"
#include "common.h"

__NAMESPACE_BEGIN

epoller_event::epoller_event(k::event_loop *_loop, k::socket *_s)
    : m_loop(_loop), m_socket(_s), m_flags(0)
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

void
epoller_event::set_pollin (bool _en)
{
    m_flags = _en ? m_flags | EPOLLIN : m_flags & ~EPOLLIN;
    update();
}

void
epoller_event::set_pollout (bool _en)
{
    m_flags = _en ? m_flags | EPOLLOUT : m_flags & ~EPOLLOUT;
    update();
}

void
epoller_event::set_pollerr (bool _en)
{
    m_flags = _en ? m_flags | EPOLLERR : m_flags & ~EPOLLERR;
    update();
}

void
epoller_event::set_pollpri (bool _en)
{
    m_flags = _en ? m_flags | EPOLLPRI : m_flags & ~EPOLLPRI;
    update();
}

void
epoller_event::set_pollhup (bool _en)
{
    m_flags = _en ? m_flags | EPOLLHUP : m_flags & ~EPOLLHUP;
    update();
}

void
epoller_event::set_pollonce (bool _en)
{
    m_flags = _en ? m_flags | EPOLLONESHOT : m_flags & ~EPOLLONESHOT;
    update();
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
epoller_event::pollerr () const
{
    return (m_flags & EPOLLERR);
}

bool
epoller_event::pollpri () const
{
    return (m_flags & EPOLLPRI);
}

bool
epoller_event::pollhup () const
{
    return (m_flags & EPOLLHUP);
}

bool
epoller_event::pollonce () const
{
    return (m_flags & EPOLLONESHOT);
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
}

void
epoller_event::set_write_cb (epoller_event_cb _fn)
{
    m_outcb = std::move(_fn);
}

void
epoller_event::set_error_cb (epoller_event_cb _fn)
{
    m_errcb = std::move(_fn);
}

void
epoller_event::set_ergent_cb (epoller_event_cb _fn)
{
    m_pricb = std::move(_fn);
}

void
epoller_event::set_close_cb (epoller_event_cb _fn)
{
    m_closecb = std::move(_fn);
}

void
epoller_event::handle_events ()
{
    log_debug("epoller_event::handle_events() flags = %d", m_flags);

    if (EPOLLHUP & m_flags) // RST
    {
        log_warning("event POLLHUP happend in fd %d", m_socket->fd());
        m_closecb(m_socket);
        return;
    }
    if (EPOLLERR & m_flags)
        m_closecb(m_socket);
    if (EPOLLIN & m_flags)
        m_incb(m_socket);
    if (EPOLLOUT & m_flags)
        m_outcb(m_socket);
    if (EPOLLPRI & m_flags)
        m_pricb(m_socket);
}

__NAMESPACE_END

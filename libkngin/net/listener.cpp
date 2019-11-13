#include <functional>
#include "define.h"
#include "socket.h"
#include "listener.h"
#include "sockopts.h"
#include "common.h"

__NAMESPACE_BEGIN

listener::listener (event_loop *_loop, k::socket &&_socket)
    try
    : m_loop(_loop),
      m_socket(std::move(_socket)),
      m_event(_loop, &m_socket),
      m_closed(false),
      m_listen_addr(),
      m_accept_cb(nullptr),
      m_error_cb(nullptr)
{
    check(_loop);
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    m_event.set_read_cb(std::bind(&listener::on_accept, this));
    m_event.set_error_cb(std::bind(&listener::on_error, this));
} catch (...) {
    log_fatal("tcp_connection::tcp_connection() error");
    throw;
}

listener::~listener()
{
    if (!m_closed)
        log_error("the listener must be closed before object disconstructing");

    // FIXME; wait for m_closed to be true
}

bool
listener::bind (const address &_listen_addr)
{
    m_listen_addr = _listen_addr;
    if (m_socket.bind(_listen_addr) < 0) {
        log_error("socket::bind() error - %s:%d", strerror(errno), errno);
        return false;
    }
    return true;
}

bool
listener::listen (int _backlog)
{
    if (m_socket.listen(_backlog) < 0) {
        log_error("socket::listen() error - %s:%d", strerror(errno), errno);
        return false;
    }
    m_event.start();
    return true;
}

void
listener::close ()
{
    check(!m_closed);

    if (m_loop->in_loop_thread())
        on_close();
    else
        m_loop->run_in_loop(std::bind(&listener::on_close, this));
}

void
listener::on_accept ()
{
    address _peer_addr;
    int _fd = m_socket.accept(_peer_addr);
    if (_fd < 0) {
        log_error("socket::accept() error - %s:%d", strerror(errno), errno);
        return;
    }

    if (m_accept_cb)
        m_accept_cb(socket(_fd));
    else {
        log_warning("unaccepted fd %d", _fd);
        ::close(_fd);
    }
}

void
listener::on_close ()
{
    check(!m_closed);
    m_loop->check_thread();

    m_event.remove();
    m_socket.close();
    m_closed = true;
}

void
listener::on_error ()
{
    check(!m_closed);
    m_loop->check_thread();

    int _err_code = 0;
    if (!sockopts::error(m_socket, _err_code))
        log_error("sockopts::error() error");
    else
        log_error("handled an socket error, fd = %d - %s:%d", m_socket.fd(), strerror(errno), errno);
    if (m_error_cb)
        m_error_cb(std::ref(*this));
}

__NAMESPACE_END

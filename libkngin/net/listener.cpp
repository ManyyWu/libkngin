#ifdef _WIN32
#else
#include <fcntl.h>
#endif
#include <functional>
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
      m_error_cb(nullptr),
      m_idle_file(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    check(_loop);
    if (!m_idle_file.valid()) {
        log_error("::open(\"/dev/null\") error - %s:%d", strerror(errno), errno);
        throw exception("::open() error");
    }
    if (!m_socket.set_closeexec(true)) {
        log_error("socket::set_closeexec(true) error");
        throw exception("socket::set_closeexec() error");
    }
    if (!m_socket.set_nonblock(true)) {
        log_error("socket::set_nonblock(true) error");
        throw exception("socket::set_nonblock() error");
    }
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
    check(!m_closed);

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
    check(!m_closed);

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

    m_idle_file.close();
    if (m_loop->in_loop_thread())
        on_close();
    else
        m_loop->run_in_loop(std::bind(&listener::on_close, this));
}

void
listener::on_accept ()
{
    check(!m_closed);
    m_loop->check_thread();

    address _peer_addr;
    int _fd = m_socket.accept(_peer_addr);
    if (_fd < 0) {
        if (EMFILE == _fd) {
            m_idle_file.close();
            m_idle_file = m_socket.accept(_peer_addr);
            m_idle_file.close();
            m_idle_file = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
            log_warning("the process already has the maximum number of files open, "
                        "a new connection has been rejected");
        } else {
            log_error("socket::accept() error - %s:%d", strerror(errno), errno);
        }
        return;
    }

    if (m_accept_cb)
        m_accept_cb(socket(_fd));
    else {
        log_warning("unaccepted connection, fd = %d", _fd);
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

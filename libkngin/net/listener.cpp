#ifdef _WIN32
#else
#include <fcntl.h>
#endif
#include <functional>
#include "core/common.h"
#include "core/system_error.h"
#include "net/sockopts.h"
#include "net/listener.h"

KNGIN_NAMESPACE_K_BEGIN

listener::listener (event_loop_ptr _loop, k::socket &&_socket)
    try
    : m_loop(std::move(_loop)),
      m_socket(std::move(_socket)),
      m_event(m_loop->pimpl(), &m_socket),
      m_closed(false),
      m_listen_addr(),
      m_accept_cb(nullptr),
      m_error_cb(nullptr),
      m_idle_file(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    check(_loop);
    if (!m_idle_file.valid())
        throw k::system_error("::open(\"/dev/null\") error");
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    m_event.set_read_cb(std::bind(&listener::on_accept, this));
    m_event.set_error_cb(std::bind(&listener::on_error, this));
} catch (...) {
    log_fatal("listener::listener() error");
    throw;
}

listener::~listener() KNGIN_NOEXP
{
    if_not (m_closed)
        log_error("the listener must be closed before object disconstructing");

    // FIXME; wait for m_closed to be true
}

void
listener::close (error_handler &&_cb)
{
    assert(!m_closed);

    m_idle_file.close();
    if (m_loop->in_loop_thread())
        on_close();
    else
        m_loop->run_in_loop(std::bind(&listener::on_close, this));
}

void
listener::on_accept () KNGIN_NOEXP
{
    if (m_closed)
        return;
    m_loop->check_thread();

    address _peer_addr;
    std::error_code _ec;
    socket _sock(m_socket.accept(_peer_addr, _ec));
    if (_ec) {
        if (std::errc::too_many_files_open == _ec) {
            ignore_exp(
                m_idle_file.close();
                m_idle_file = m_socket.accept(_peer_addr);
                m_idle_file.close();
                m_idle_file = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
            );
            log_warning("the process already has the maximum number of files open, "
                        "a new session has been rejected");
        } else {
            log_error("socket::accept() error - %s:%d", _ec.message().c_str(), _ec.value());
#warning "process error code"
        }
        return;
    }

    ignore_exp(
        if (m_accept_cb) {
            m_accept_cb(std::move(_sock));
        } else {
            log_warning("unaccepted session, fd = %d", _sock.fd());
            _sock.close();
        }
    );
}

void
listener::on_close () KNGIN_NOEXP
{
    assert(!m_closed);
    m_loop->check_thread();

    ignore_exp(
        m_event.remove();
        m_socket.close();
    );
    m_closed = true;
}

void
listener::on_error () KNGIN_NOEXP
{
    assert(!m_closed);
    m_loop->check_thread();

    std::error_code _error, _ec;
    _error = sockopts::error(m_socket, _ec);
    if (_ec)
        log_error("%s - %d", _ec.message().c_str(), _ec.value());
    if (m_error_cb)
        ignore_exp(m_error_cb(_error));
}

KNGIN_NAMESPACE_K_END

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

listener::pimpl::pimpl (event_loop &_loop, k::socket &&_socket)
    try
    : epoller_event(_socket.dup()),
      m_loop(_loop.pimpl()),
      m_socket(std::move(_socket)),
      m_closed(true),
      m_listen_addr(),
      m_accept_handler(nullptr),
      m_close_handler(nullptr),
      m_idle_file(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    check(m_loop);
    if (!m_idle_file.valid())
        throw k::system_error("::open(\"/dev/null\") error");
    m_socket.set_closeexec(true);
    sockopts::set_reuseaddr(m_socket, true);
    sockopts::set_reuseport(m_socket, true);
    enable_read();
    m_closed = false;
} catch (...) {
    log_fatal("listener::listener() error");
    throw;
}

listener::pimpl::~pimpl () KNGIN_NOEXP
{
    if (!m_closed)
        ignore_exp(this->close(true));

    // FIXME; wait for m_closed to be true
}

void
listener::pimpl::bind (const address &_listen_addr)
{
    check(!m_closed);
    m_socket.bind(m_listen_addr = _listen_addr);
    m_socket.set_nonblock(true);
}

void
listener::pimpl::bind (const address &_listen_addr, std::error_code &_ec) KNGIN_NOEXP
{
    check(!m_closed);
    m_socket.bind(m_listen_addr = _listen_addr, _ec);
    m_socket.set_nonblock(true);
}

void
listener::pimpl::listen (int _backlog,
                         accept_handler &&_new_ssesion_handler,
                         close_handler &&_close_handler)
{
    check(!m_closed);
    m_accept_handler = std::move(_new_ssesion_handler); 
    m_close_handler = std::move(_close_handler);
    m_socket.listen(_backlog);
}

void
listener::pimpl::listen (int _backlog, std::error_code &_ec,
                         accept_handler &&_new_sesssion_handler,
                         close_handler &&_close_handler) KNGIN_NOEXP
{
    check(!m_closed);
    m_accept_handler = std::move(_new_sesssion_handler); 
    m_close_handler = std::move(_close_handler);
    m_socket.listen(_backlog, _ec);
}

void
listener::pimpl::close (bool _blocking /* = true */)
{
    check(!m_closed);

    if (!m_loop->looping()) {
        m_socket.close();
        m_closed = true;
        return;
    }
    m_loop->remove_event(self());
    if (m_loop->in_loop_thread()) {
        on_close(std::error_code());
    } else {
        if (_blocking) {
            std::shared_ptr<barrier> _barrier_ptr = std::make_shared<barrier>(2);
            m_loop->run_in_loop([this, _barrier_ptr] () {
                on_close(std::error_code());
                if (_barrier_ptr->wait())
                    _barrier_ptr->destroy();
            });
            if (_barrier_ptr->wait())
                _barrier_ptr->destroy();
        } else {
            m_loop->run_in_loop([this] () {
                on_close(std::error_code());
            });
        }
    }
}

void
listener::pimpl::on_read ()
{
    if (m_closed)
        return;
    m_loop->check_thread();

    address _peer_addr;
    std::error_code _ec;
    int _sockfd = m_socket.accept(_peer_addr, _ec); // nonblocking
    if (_ec) {
        if (std::errc::too_many_files_open == _ec) {
            m_idle_file.close();
            m_idle_file = m_socket.accept(_peer_addr);
            m_idle_file.close();
            m_idle_file = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
            log_warning("the process already has the maximum number of files open, "
                        "a new session has been rejected");
        } else if (std::errc::resource_unavailable_try_again == _ec ||
                   std::errc::operation_would_block == _ec ||
                   std::errc::protocol_error == _ec ||
                   std::errc::connection_aborted == _ec ||
                   std::errc::interrupted == _ec) {
            log_debug("listener::on_accept() ignore error, %s",
                      system_error_str(_ec).c_str());
            return;
        } else {
            log_error("socket::accept() error, %s", system_error_str(_ec).c_str());
            on_close(_ec);
#warning "process error code, callback"
        }
        return;
    }

    socket _sock(_sockfd);
    if (m_accept_handler) {
        ignore_exp(m_accept_handler(std::move(_sock)));
    } else {
        log_warning("unaccepted session, fd = %d", _sock.fd());
        _sock.close();
    }
}

void
listener::pimpl::on_close (std::error_code _ec)
{
    check(!m_closed);
    m_loop->check_thread();

    if (m_loop->registed(self()))
        m_loop->remove_event(self());
    m_socket.close();
    m_closed = true;

    if (m_close_handler)
        ignore_exp(m_close_handler(_ec));
}

void
listener::pimpl::on_error ()
{
    check(!m_closed);
    m_loop->check_thread();

    on_read();
}

KNGIN_NAMESPACE_K_END

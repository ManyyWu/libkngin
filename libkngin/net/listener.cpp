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
    : m_loop(_loop),
      m_socket(std::move(_socket)),
      m_event(m_loop->pimpl(), &m_socket),
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
    m_event.set_read_handler(std::bind(&listener::on_accept, this));
    m_event.set_error_handler(std::bind(&listener::on_error, this));
    m_event.start();
    m_closed = false;
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
listener::bind (const address &_listen_addr)
{
    check(!m_closed);
    m_socket.bind(m_listen_addr = _listen_addr);
    m_socket.set_nonblock(true);
}

void
listener::bind (const address &_listen_addr, std::error_code &_ec) KNGIN_NOEXP
{
    check(!m_closed);
    m_socket.bind(m_listen_addr = _listen_addr, _ec);
    m_socket.set_nonblock(true);
}

void
listener::listen (int _backlog, accept_handler &&_new_ssesion_handler, 
                  close_handler &&_close_handler)
{
    check(!m_closed);
    m_accept_handler = std::move(_new_ssesion_handler); 
    m_close_handler = std::move(_close_handler);
    m_socket.listen(_backlog);
}

void
listener::listen (int _backlog, std::error_code &_ec, 
                  accept_handler &&_new_sesssion_handler, 
                  close_handler &&_close_handler) KNGIN_NOEXP
{
    check(!m_closed);
    m_accept_handler = std::move(_new_sesssion_handler); 
    m_close_handler = std::move(_close_handler);
    m_socket.listen(_backlog, _ec);
}

void
listener::close ()
{
    check(!m_closed);

    m_idle_file.close();
    if (m_loop->in_loop_thread())
        on_close(std::error_code());
    else
        m_loop->run_in_loop([this] () {
            on_close(std::error_code());
        });
}

void
listener::on_accept () KNGIN_NOEXP
{
    if (m_closed)
        return;
    m_loop->check_thread();

    address _peer_addr;
    std::error_code _ec;
    int _sockfd = m_socket.accept(_peer_addr, _ec); // nonblocking
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
        } else if (std::errc::resource_unavailable_try_again == _ec ||
                   std::errc::operation_would_block == _ec ||
                   std::errc::protocol_error == _ec ||
                   std::errc::connection_aborted == _ec ||
                   std::errc::interrupted == _ec) {
            log_debug("listener::on_accept() ignore error - %s",
                      system_error_str(_ec).c_str());
            return;
        } else {
            log_error("socket::accept() error - %s", system_error_str(_ec).c_str());
            on_close(_ec);
#warning "process error code, callback"
        }
        return;
    }

    socket _sock(_sockfd);
    ignore_exp(
        if (m_accept_handler) {
            m_accept_handler(std::move(_sock));
        } else {
            log_warning("unaccepted session, fd = %d", _sock.fd());
            _sock.close();
        }
    );
}

void
listener::on_close (std::error_code _ec) KNGIN_NOEXP
{
    check(!m_closed);
    m_loop->check_thread();

    ignore_exp(
        m_event.remove();
        m_socket.close();

        if (m_close_handler)
            m_close_handler(_ec);
    );
    m_closed = true;
}

void
listener::on_error () KNGIN_NOEXP
{
    check(!m_closed);
    m_loop->check_thread();

    on_accept();
}

KNGIN_NAMESPACE_K_END

#ifdef _WIN32
#else
#include <fcntl.h>
#include <netdb.h>
#endif
#include <functional>
#include "core/common.h"
#include "core/system_error.h"
#include "net/sockopts.h"
#include "net/listener.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/listener.cpp"

KNGIN_NAMESPACE_K_BEGIN

listener::listener (event_loop &_loop, k::socket &&_socket,
                    const std::string &_name, uint16_t _port,
                    int _backlog,
                    accept_handler &&_new_ssesion_handler,
                    error_handler &&_error_handler)
    try
    : epoller_event(_socket.fd(), epoller_event::EVENT_TYPE_FILE),
      m_loop(_loop.pimpl()),
      m_socket(std::move(_socket)),
      m_closed(true),
      m_listen_addr(),
      m_accept_handler(std::move(_new_ssesion_handler)),
      m_error_handler(std::move(_error_handler)),
      m_idle_file(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    arg_check(m_socket.valid());
    if (!m_idle_file.valid())
        throw k::system_error("::open(\"/dev/null\") error");

    // parse address
    parse_addr(_name, _port);

    // set socket options
    sockopts::set_reuseaddr(m_socket, true);
    sockopts::set_reuseport(m_socket, true);

    // bind
    m_socket.bind(m_listen_addr);

    // listen
    m_socket.listen(_backlog);

    // set socket flags
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);

    // set event falgs
    enable_read();
    m_closed = false;
} catch (...) {
    log_fatal("listener::listener() error");
    throw;
}

listener::~listener () KNGIN_NOEXCP
{
    if (!m_closed && registed())
        ignore_excp(this->close(true));

    // FIXME; wait for m_closed to be true
}

void
listener::parse_addr (const std::string &_name, uint16_t _port)
{
    addrinfo   _ai;
    addrinfo * _ai_list;
    ::bzero(&_ai, sizeof(addrinfo));
    _ai.ai_flags = AI_PASSIVE;
    _ai.ai_family = AF_UNSPEC;
    _ai.ai_protocol = 0;
    int _ret = ::getaddrinfo(_name.c_str(),
                             std::to_string(_port).c_str(),
                             &_ai, &_ai_list);
    if (_ret) {
        if (EAI_SYSTEM == _ret)
            throw k::system_error("::getaddrinfo() error");
        else
            throw k::exception((std::string("::getaddrinfo() error, %s")
                                + gai_strerror(_ret) ).c_str());
    }
    if_not (_ai_list) {
        ::freeaddrinfo(_ai_list);
        throw k::exception("invalid name or port");
    }
    if (AF_INET == _ai_list->ai_addr->sa_family)
        m_listen_addr = *(sockaddr_in *)_ai_list->ai_addr;
    else if (AF_INET6 == _ai_list->ai_addr->sa_family)
        m_listen_addr = *(sockaddr_in6 *)_ai_list->ai_addr;
    else
        throw k::exception("unsupported address family");

    //addrinfo *_res = _ai_list;
    //while (_res) {
    //    ::getnameinfo(_res->ai_addr, _res->ai_addrlen, nullptr, 0, nullptr, 0, 0);
    //    _res = _res->ai_next;
    //}

    ::freeaddrinfo(_ai_list);
}

void
listener::close (bool _blocking /* = true */)
{
    if (m_closed)
        return;
        
    if (!m_loop->looping()) {
        goto dir_close;
    }
    if (registed()) {
        if (m_loop->in_loop_thread()) {
            on_close();
        } else {
            m_loop->remove_event(*this);
            if (_blocking) {
                std::shared_ptr<barrier> _barrier_ptr = std::make_shared<barrier>(2);
                m_loop->run_in_loop([this, _barrier_ptr] () {
                    on_close();
                    if (_barrier_ptr->wait())
                        _barrier_ptr->destroy();
                });
                if (_barrier_ptr->wait())
                    _barrier_ptr->destroy();
            } else {
                m_loop->run_in_loop([this] () {
                    on_close();
                });
            }
        }
    } else {
dir_close:
        m_socket.close();
        m_closed = true;
    }
}

void
listener::on_read ()
{
    if (m_closed)
        return;
    assert(m_loop->in_loop_thread());

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
            log_excp_error(
                m_error_handler(_ec),
                "listener::m_accept_handler() error"
            );
        }
        return;
    }

    socket _sock(_sockfd);
    if (m_accept_handler) {
        log_excp_error(
            m_accept_handler(std::move(_sock)),
            "listener::m_accept_handler() error"
        );
    } else {
        log_warning("unaccepted session, fd = %d", _sock.fd());
        _sock.close();
    }
}

void
listener::on_error ()
{
    assert(!m_closed);
    assert(m_loop->in_loop_thread());

    on_read();
}

void
listener::on_close ()
{
    if (m_closed)
        return;
    assert(m_loop->in_loop_thread());

    auto _self = self(); // extend the life cycle untile closed
    if (m_loop->looping() && registed())
        m_loop->remove_event(*this);
    m_socket.close();
    m_closed = true;
}

KNGIN_NAMESPACE_K_END

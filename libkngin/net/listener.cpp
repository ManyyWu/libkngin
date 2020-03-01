#ifdef _WIN32
#else
#include <fcntl.h>
#include <netdb.h>
#endif
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "net/sockopts.h"
#include "net/listener.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/listener.cpp"

KNGIN_NAMESPACE_K_BEGIN

listener::listener (event_loop &loop, k::socket &&socket,
                    const std::string &name, uint16_t port,
                    int backlog,
                    accept_handler &&new_ssesion_handler,
                    error_handler &&error_handler)
    try
    : epoller_event(socket.fd(), epoller_event::EVENT_TYPE_FILE),
      m_loop(&loop),
      m_socket(std::move(socket)),
      m_closed(true),
      m_listen_addr(),
      m_accept_handler(std::move(new_ssesion_handler)),
      m_error_handler(std::move(error_handler)),
      m_idle_file(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    arg_check(m_socket.valid());
    if (!m_idle_file.valid())
        throw k::system_error("::open(\"/dev/null\") error");

    // parse address
    parse_addr(name, port);

    // set socket options
    sockopts::set_reuseaddr(m_socket, true);
#ifndef _WIN32
    sockopts::set_reuseport(m_socket, true);
#endif

    // bind
    m_socket.bind(m_listen_addr);

    // listen
    m_socket.listen(backlog);

    // set file flags
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);

    // set event falgs
    enable_read();
    m_closed = false;
} catch (...) {
    log_fatal("listener::listener() error");
    throw;
}

listener::~listener () noexcept
{
    if (!m_closed and registed())
        ignore_excp(this->close(true));

    // FIXME; wait for m_closed to be true
}

void
listener::parse_addr (const std::string &name, uint16_t port)
{
    addrinfo   ai;
    addrinfo * ai_list;
    ::bzero(&ai, sizeof(addrinfo));
    ai.ai_flags = AI_PASSIVE;
    ai.ai_family = AF_UNSPEC;
    ai.ai_protocol = 0;
    int ret = ::getaddrinfo(name.c_str(),
                             std::to_string(port).c_str(),
                             &ai, &ai_list);
    if (ret) {
        if (EAI_SYSTEM == ret)
            throw k::system_error("::getaddrinfo() error");
        else
            throw k::exception((std::string("::getaddrinfo() error, %s")
                                + gai_strerror(ret) ).c_str());
    }
    if_not (ai_list) {
        ::freeaddrinfo(ai_list);
        throw k::exception("invalid name or port");
    }
    if (AF_INET == ai_list->ai_addr->sa_family)
        m_listen_addr = *(sockaddr_in *)ai_list->ai_addr;
    else if (AF_INET6 == ai_list->ai_addr->sa_family)
        m_listen_addr = *(sockaddr_in6 *)ai_list->ai_addr;
    else
        throw k::exception("unsupported address family");

    //addrinfo *res = ai_list;
    //while (res) {
    //    ::getnameinfo(res->ai_addr, res->ai_addrlen, nullptr, 0, nullptr, 0, 0);
    //    res = res->ai_next;
    //}

    ::freeaddrinfo(ai_list);
}

void
listener::close (bool sync /* = true */)
{
    if (m_closed)
        return;

    if (registed() and m_loop and m_loop->looping()) {
        if (m_loop->in_loop_thread()) {
            on_close();
        } else {
            m_loop->remove_event(*this);
            auto self_weak_ptr = weak_ptr();
            if (sync) {
                auto barrier_ptr = std::make_shared<barrier>(2);
                m_loop->run_in_loop([self_weak_ptr, barrier_ptr] () {
                    auto s = self_weak_ptr.lock();
                    if (s)
                        s->on_close();
                    if (barrier_ptr->wait())
                        barrier_ptr->destroy();
                });
                if (barrier_ptr->wait())
                    barrier_ptr->destroy();
            } else {
                m_loop->run_in_loop([self_weak_ptr] () {
                    auto s = self_weak_ptr.lock();
                    if (s)
                        s->on_close();
                });
            }
        }
    } else {
        m_socket.close();
        m_closed = true;
    }
}

void
listener::on_events (event_loop &loop, uint32_t flags)
{
    assert(m_loop == &loop);
    try {
        if ((EPOLLHUP | EPOLLERR) & flags) {
            this->on_read();
            return;
        }
        if (EPOLLIN & flags)
            this->on_read();
    } catch (std::exception &e) {
        log_fatal("caught an exception in listener::on_event(), %s", e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in listener::on_event()");
        throw;
    }
}

void
listener::on_read ()
{
    if (m_closed)
        return;
    auto s = self();

    address peer_addr;
    error_code ec;
    int sockfd = m_socket.accept(peer_addr, ec); // nonblocking
    if (ec) {
        if (EMFILE == ec) {
            m_idle_file.close();
            m_idle_file = m_socket.accept(peer_addr);
            m_idle_file.close();
            m_idle_file = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
            log_warning("the process already has the maximum number of files open, "
                        "a new session has been rejected");
        } else if (EAGAIN == ec or
                   EWOULDBLOCK == ec or
                   EPROTO == ec or
                   ECONNABORTED == ec or
                   EINTR == ec) {
            log_debug("listener::on_accept() ignore error, %s",
                      system_error_str(ec).c_str());
            return;
        } else {
            log_excp_error(
                m_error_handler(ec),
                "listener::m_accept_handler() error"
            );
        }
        return;
    }

    socket sock(sockfd);
    if (m_accept_handler) {
        log_excp_error(
            m_accept_handler(std::move(sock)),
            "listener::m_accept_handler() error"
        );
    } else {
        log_warning("unaccepted session, fd = %d", sock.fd());
        sock.close();
    }
}

void
listener::on_close ()
{
    if (m_closed)
        return;

    auto s = self();
    if (m_loop and m_loop->looping() and registed())
        m_loop->remove_event(*this);
    m_socket.close();
    m_closed = true;
}

KNGIN_NAMESPACE_K_END

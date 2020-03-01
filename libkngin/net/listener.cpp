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
      loop_(&loop),
      socket_(std::move(socket)),
      closed_(true),
      listen_addr_(),
      accept_handler_(std::move(new_ssesion_handler)),
      error_handler_(std::move(error_handler)),
      idle_file_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    arg_check(socket_.valid());
    if (!idle_file_.valid())
        throw k::system_error("::open(\"/dev/null\") error");

    // parse address
    parse_addr(name, port);

    // set socket options
    sockopts::set_reuseaddr(socket_, true);
#ifndef _WIN32
    sockopts::set_reuseport(socket_, true);
#endif

    // bind
    socket_.bind(listen_addr_);

    // listen
    socket_.listen(backlog);

    // set file flags
    socket_.set_closeexec(true);
    socket_.set_nonblock(true);

    // set event falgs
    enable_read();
    closed_ = false;
} catch (...) {
    log_fatal("listener::listener() error");
    throw;
}

listener::~listener () noexcept
{
    if (!closed_ and registed())
        ignore_excp(this->close(true));

    // FIXME; wait for closed_ to be true
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
        listen_addr_ = *(sockaddr_in *)ai_list->ai_addr;
    else if (AF_INET6 == ai_list->ai_addr->sa_family)
        listen_addr_ = *(sockaddr_in6 *)ai_list->ai_addr;
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
    if (closed_)
        return;

    if (registed() and loop_ and loop_->looping()) {
        if (loop_->in_loop_thread()) {
            on_close();
        } else {
            loop_->remove_event(*this);
            auto self_weak_ptr = weak_ptr();
            if (sync) {
                auto barrier_ptr = std::make_shared<barrier>(2);
                loop_->run_in_loop([self_weak_ptr, barrier_ptr] () {
                    auto s = self_weak_ptr.lock();
                    if (s)
                        s->on_close();
                    if (barrier_ptr->wait())
                        barrier_ptr->destroy();
                });
                if (barrier_ptr->wait())
                    barrier_ptr->destroy();
            } else {
                loop_->run_in_loop([self_weak_ptr] () {
                    auto s = self_weak_ptr.lock();
                    if (s)
                        s->on_close();
                });
            }
        }
    } else {
        socket_.close();
        closed_ = true;
    }
}

void
listener::on_events (event_loop &loop, uint32_t flags)
{
    assert(loop_ == &loop);
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
    if (closed_)
        return;
    auto s = self();

    address peer_addr;
    error_code ec;
    int sockfd = socket_.accept(peer_addr, ec); // nonblocking
    if (ec) {
        if (EMFILE == ec) {
            idle_file_.close();
            idle_file_ = socket_.accept(peer_addr);
            idle_file_.close();
            idle_file_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
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
                error_handler_(ec),
                "listener::accept_handler_() error"
            );
        }
        return;
    }

    socket sock(sockfd);
    if (accept_handler_) {
        log_excp_error(
            accept_handler_(std::move(sock)),
            "listener::accept_handler_() error"
        );
    } else {
        log_warning("unaccepted session, fd = %d", sock.fd());
        sock.close();
    }
}

void
listener::on_close ()
{
    if (closed_)
        return;

    auto s = self();
    if (loop_ and loop_->looping() and registed())
        loop_->remove_event(*this);
    socket_.close();
    closed_ = true;
}

KNGIN_NAMESPACE_K_END

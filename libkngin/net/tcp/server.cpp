#ifdef _WIN32
#else
//#include <signal.h>
#endif
#include "core/base/exception.h"
#include "core/base/system_error.h"
#include "core/base/common.h"
#include "net/tcp/server.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/tcp/server.cpp"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

server::server (event_loop &loop, const server_opts &opts)
    try 
    : loop_(&loop),
      opts_(opts),
      threadpool_(opts.thread_num),
      listener_(nullptr),
      sent_handler_(nullptr),
      message_handler_(nullptr),
      oob_handler_(nullptr),
      error_handler_(nullptr),
      crash_handler_(nullptr),
      stopped_(true),
      stopping_(false)
{
    if (opts.disable_debug)
        logger()[0].disable_debug();
    if (opts.disable_info)
        logger()[0].disable_info();
    if (!opts.allow_ipv4 and !opts.allow_ipv6)
        throw k::exception("invalid options");

    // check address
    auto pos = opts.name.find_first_of('%');
    auto s = (pos != std::string::npos)
                  ? std::string(opts.name.data(), pos)
                  : opts.name;
    if (opts.allow_ipv4 and
        opts.allow_ipv6 and
        !address::is_valid_inet6_addrstr(s)
        )
        throw k::exception("invalid ipv6 address");
    if (opts.allow_ipv4 and
        !opts.allow_ipv6 and
        !address::is_valid_inet_addrstr(opts.name)
        )
        throw k::exception("invalid ipv4 address");
} catch (...) {
    log_fatal("server::server() error");
    throw;
}

server::~server () noexcept
{
    if (!stopped_)
        ignore_excp(stop());
}

bool
server::run ()
{
    assert(stopped_);

    auto crash_handler = [this] () {
        loop_->run_in_loop([this] () {
            if (crash_handler_) {
                log_excp_fatal(
                    crash_handler_(),
                    "server::crash_hander_() error"
                );
            }
        }); // run in main thread
    }; // end of crash_handler, run in thread pool

    // shielding SIGPIPE signal
/*
#ifndef _WIN32
    sigset_t signal_mask;
    ::sigemptyset(&signal_mask);
    ::sigaddset(&signal_mask, SIGPIPE);
    ::pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
#endif
*/

    // run threadpool
    threadpool_.start(crash_handler);

    // create listen socket
    socket listener_sock(opts_.allow_ipv6
                          ? socket::IPV6_TCP
                          : socket::IPV4_TCP);
    if (opts_.allow_ipv6 and !opts_.allow_ipv4)
        sockopts::set_ipv6_only(listener_sock, true);

    // start listener
    auto on_new_session = [this] (socket &&sock) {
        assert(!stopped_);
        // FIXME: stop accepting when sessions reach to the maximum number

        try {
            auto local_addr = sock.localaddr();
            auto peer_addr = sock.peeraddr();

            session_ptr new_session = nullptr;
            auto &next_loop = assign_thread();
            new_session = std::make_shared<session>(next_loop,
                                                std::move(sock),
                                                local_addr, peer_addr);
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
            session->set_message_handler(message_handler_);
             session->set_sent_handler(sent_handler_);
#endif

            // run in self loop
            if (session_handler_) {
                next_loop.run_in_loop([this, new_session ] () {
                    auto next_loop = new_session->loop();
                    assert(next_loop);
                    new_session->set_oob_handler(oob_handler_);
                    new_session->set_error_handler(error_handler_);
                    if (opts_.keep_alive)
                        new_session->set_keepalive(true);
                    next_loop->register_event(new_session);
                    log_excp_error(
                        session_handler_(new_session),
                        "server::session_handler_() error"
                    );
                });
            }
        } catch (const std::exception &e) {
            log_error("caught an exception when accepting new session, %s", e.what());
            throw;
        } catch (...) {
            log_fatal("caught an undefined exception when accepting new session");
            throw;
        }
    }; // end of on_new_session

    auto on_listener_error = [this, crash_handler] (error_code ec) {
        assert(!stopped_);
        if (ec) {
            log_error("listener error, %s", system_error_str(ec).c_str());
            crash_handler();
        }
    }; // end of on_listener_error, run in listener thread

    listener_ = std::make_shared<listener>(threadpool_.get_loop(0),
                                            std::move(listener_sock),
                                            opts_.name, opts_.port,
                                            opts_.backlog,
                                            std::move(on_new_session),
                                            std::move(on_listener_error));
    threadpool_.get_loop(0).register_event(listener_);
    log_info("listening for [[%s]:%d]",
             listener_->listen_addr().addrstr().c_str(),
             listener_->listen_addr().port());

    stopped_ = false;
    log_info("TCP server is running");
    return true;
}

void
server::stop ()
{
    if (stopped_ or stopping_)
        return;
    stopping_ = true;
    log_info("stopping TCP server");

    // close listener
    if (listener_)
        listener_->close(true);

    // close all threads but self
    if (!stopped_)
        threadpool_.stop();
    stopped_ = true;
    log_info("TCP server has stopped");
}

void
server::broadcast (session_list &list, msg_buffer buf)
{
    assert(list.size());
    assert(!stopped_);

    if (stopping_)
        return;

    for (auto &iter : list) {
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
        iter->send(msg_buffer(buf.get(), 0, buf.buffer().size()));
#else
        loop_->run_in_loop([buf, iter] () {
            iter->send(buf, nullptr);
        }); // run in the loop corresponding to the connection
#endif
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

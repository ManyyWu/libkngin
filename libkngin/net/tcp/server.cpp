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
    : m_loop(&loop),
      m_opts(opts),
      m_threadpool(opts.thread_num),
      m_listener(nullptr),
      m_sent_handler(nullptr),
      m_message_handler(nullptr),
      m_oob_handler(nullptr),
      m_error_handler(nullptr),
      m_crash_handler(nullptr),
      m_stopped(true),
      m_stopping(false)
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
    if (!m_stopped)
        ignore_excp(stop());
}

bool
server::run ()
{
    assert(m_stopped);

    auto crash_handler = [this] () {
        m_loop->run_in_loop([this] () {
            if (m_crash_handler) {
                log_excp_fatal(
                    m_crash_handler(),
                    "server::m_crash_hander() error"
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
    m_threadpool.start(crash_handler);

    // create listen socket
    socket listener_sock(m_opts.allow_ipv6
                          ? socket::IPV6_TCP
                          : socket::IPV4_TCP);
    if (m_opts.allow_ipv6 and !m_opts.allow_ipv4)
        sockopts::set_ipv6_only(listener_sock, true);

    // start listener
    auto on_new_session = [this] (socket &&sock) {
        assert(!m_stopped);
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
            session->set_message_handler(m_message_handler);
             session->set_sent_handler(m_sent_handler);
#endif

            // run in self loop
            if (m_session_handler) {
                next_loop.run_in_loop([this, new_session ] () {
                    auto next_loop = new_session->loop();
                    assert(next_loop);
                    new_session->set_oob_handler(m_oob_handler);
                    new_session->set_error_handler(m_error_handler);
                    if (m_opts.keep_alive)
                        new_session->set_keepalive(true);
                    next_loop->register_event(new_session);
                    log_excp_error(
                        m_session_handler(new_session),
                        "server::m_session_handler() error"
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
        assert(!m_stopped);
        if (ec) {
            log_error("listener error, %s", system_error_str(ec).c_str());
            crash_handler();
        }
    }; // end of on_listener_error, run in listener thread

    m_listener = std::make_shared<listener>(m_threadpool.get_loop(0),
                                            std::move(listener_sock),
                                            m_opts.name, m_opts.port,
                                            m_opts.backlog,
                                            std::move(on_new_session),
                                            std::move(on_listener_error));
    m_threadpool.get_loop(0).register_event(m_listener);
    log_info("listening for [[%s]:%d]",
             m_listener->listen_addr().addrstr().c_str(),
             m_listener->listen_addr().port());

    m_stopped = false;
    log_info("TCP server is running");
    return true;
}

void
server::stop ()
{
    if (m_stopped or m_stopping)
        return;
    m_stopping = true;
    log_info("stopping TCP server");

    // close listener
    if (m_listener)
        m_listener->close(true);

    // close all threads but self
    if (!m_stopped)
        m_threadpool.stop(); 
    m_stopped = true;
    log_info("TCP server has stopped");
}

void
server::broadcast (session_list &list, msg_buffer buf)
{
    assert(list.size());
    assert(!m_stopped);

    if (m_stopping)
        return;

    for (auto &iter : list) {
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
        iter->send(msg_buffer(buf.get(), 0, buf.buffer().size()));
#else
        m_loop->run_in_loop([buf, iter] () {
            iter->send(buf, nullptr);
        }); // run in the loop corresponding to the connection
#endif
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

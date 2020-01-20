#ifdef _WIN32
#else
#include <unistd.h>
#include <signal.h>
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

server::server (event_loop &_loop, const server_opts &_opts)
    try 
    : m_loop(&_loop),
      m_opts(_opts),
      m_threadpool(_opts.thread_num),
      m_listener(nullptr),
      m_sent_handler(nullptr),
      m_message_handler(nullptr),
      m_oob_handler(nullptr),
      m_error_handler(nullptr),
      m_crash_handler(nullptr),
      m_stopped(true),
      m_stopping(false)
{
    if (_opts.disable_debug)
        logger()[0].disable_debug();
    if (_opts.disable_info)
        logger()[0].disable_info();
    if (!_opts.allow_ipv4 and !_opts.allow_ipv6)
        throw k::exception("invalid options");

    // check address
    auto _pos = _opts.name.find_first_of('%');
    auto _s = (_pos != std::string::npos)
                  ? std::string(_opts.name.data(), _pos)
                  : _opts.name;
    if (_opts.allow_ipv4 and _opts.allow_ipv6 and !address::is_valid_inet6_addrstr(_s))
        throw k::exception("invalid ipv6 address");
    if (_opts.allow_ipv4 and !_opts.allow_ipv6 and !address::is_valid_inet_addrstr(_opts.name))
        throw k::exception("invalid ipv4 address");
} catch (...) {
    log_fatal("server::server() error");
    throw;
}

server::~server () KNGIN_NOEXCP
{
    if (!m_stopped)
        ignore_excp(stop());
}

bool
server::run ()
{
    assert(m_stopped);

    auto _crash_handler = [this] () {
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
    sigset_t _signal_mask;
    sigemptyset(&_signal_mask);
    sigaddset(&_signal_mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &_signal_mask, NULL);
#endif
*/

    // run threadpool
    m_threadpool.start(_crash_handler);

    // create listen socket
    socket _listener_sock(m_opts.allow_ipv6 ? socket::IPV6_TCP : socket::IPV4_TCP);
    if (m_opts.allow_ipv6 and !m_opts.allow_ipv4)
        sockopts::set_ipv6_only(_listener_sock, true);

    // start listener
    auto _on_new_session = [this] (socket &&_sock) {
        assert(!m_stopped);
        // FIXME: stop accepting when sessions reach to the maximum number

        try {
            auto _local_addr = _sock.localaddr();
            auto _peer_addr = _sock.peeraddr();

            session_ptr _session = nullptr;
            auto &_next_loop = assign_thread();
            _session = std::make_shared<session>(_next_loop,
                                                 std::move(_sock),
                                                 _local_addr, _peer_addr);
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
            _session->set_message_handler(m_message_handler);
             _session->set_sent_handler(m_sent_handler);
#endif

            // run in self loop
            if (m_session_handler) {
                _next_loop.run_in_loop([this, _session] () {
                    auto _next_loop = _session->loop();
                    assert(_next_loop);
                    _session->set_oob_handler(m_oob_handler);
                    _session->set_error_handler(m_error_handler);
                    if (m_opts.keep_alive)
                        _session->set_keepalive(true);
                    _next_loop->register_event(_session);
                    log_excp_error(
                        m_session_handler(_session),
                        "server::m_session_handler() error"
                    );
                });
            }
        } catch (const std::exception &_e) {
            log_error("caught an exception when accepting new session, %s", _e.what());
            throw;
        } catch (...) {
            log_fatal("caught an undefined exception when accepting new session");
            throw;
        }
    }; // end of on_new_session

    auto _on_listener_error = [this, _crash_handler] (std::error_code _ec) {
        assert(!m_stopped);
        if (_ec) {
            log_error("listener error, %s", system_error_str(_ec).c_str());
            _crash_handler();
        }
    }; // end of on_listener_error, run in listener thread

    m_listener = std::make_shared<listener>(m_threadpool.get_loop(0),
                                            std::move(_listener_sock),
                                            m_opts.name, m_opts.port,
                                            m_opts.backlog,
                                            std::move(_on_new_session),
                                            std::move(_on_listener_error));
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
server::broadcast (session_list &_list, msg_buffer _buf)
{
    assert(_list.size());
    assert(!m_stopped);

    if (m_stopping)
        return;

    for (auto &_iter : _list) {
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
        _iter->send(msg_buffer(_buf.get(), 0, _buf.buffer().size()));
#else
        m_loop->run_in_loop([_buf, _iter] () {
            _iter->send(_buf, nullptr);
        }); // run in the loop corresponding to the connection
#endif
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

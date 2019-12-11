#ifdef _WIN32
#else
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#endif
#include "core/exception.h"
#include "core/system_error.h"
#include "core/common.h"
#include "net/tcp/server.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

server::server (const server_opts &_opts)
    try 
    : m_opts(_opts),
      m_threadpool(_opts.thread_num),
      m_sessions(),
      m_listener(nullptr),
      m_listen_addr(),
      m_sent_handler(nullptr),
      m_message_handler(nullptr),
      m_oob_handler(nullptr),
      m_close_handler(nullptr),
      m_crash_handler(nullptr),
      m_stopped(true),
      m_stopping(false),
      m_mutex()
{
    if (_opts.disable_debug)
        logger()[0].disable_debug();
    if (_opts.disable_info)
        logger()[0].disable_info();
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

    // shielding SIGPIPE signal
#ifndef _WIN32
    ::signal(SIGPIPE, SIG_IGN);
#endif

    // run threadpool
    m_threadpool.start(std::bind(&server::stop, this, std::placeholders::_1));

    // create listen socket
    socket _listener_sock(m_opts.allow_ipv6 ? socket::IPV6_TCP : socket::IPV4_TCP);

    // start listener
    m_listener = std::make_shared<listener>(m_threadpool.get_loop(0),
                                            std::move(_listener_sock));

    // init address
    log_excp_error(
        parse_addr(m_opts.name, m_opts.port),
        "address resolution failed"
    );

    // bind
    m_listener->bind(m_listen_addr);

    // listen
    m_listener->listen(m_opts.backlog, 
                       std::bind(&server::on_new_session, this,
                                 std::placeholders::_1),
                       std::bind(&server::on_listener_close, this, 
                                 std::placeholders::_1));
    m_threadpool.get_loop(0).register_event(m_listener);
    log_info("listening for [[%s]:%d]",
             m_listen_addr.addrstr().c_str(), m_listen_addr.port());

    m_stopped = false;
    log_info("TCP server is running");
    return true;
}

void
server::stop (bool _crash/* = false */)
{
    if (m_stopped)
        return;
    log_info("stopping TCP server");

    // close listener
    if (m_listener)
        m_listener->close(true);

    // close all sessions
    {
        m_stopping = true;
        local_lock _lock(m_mutex);
        for (auto _iter : m_sessions)
            _iter.second->close(true);
        m_sessions.clear();
    }

    // close all threads but self
    if (!m_stopped)
        m_threadpool.stop(); 
    m_stopped = true;
    log_info("TCP server has stopped");

    if (m_crash_handler) {
        log_fatal("TCP server has crashed");
        ignore_excp(m_crash_handler());
    }
}

size_t
server::session_num ()
{
    {
        local_lock _lock(m_mutex);
        return m_sessions.size();
    }
}

void
server::remove_session (session_ptr _session)
{
    arg_check(_session && _session->connected());
    assert(!m_stopped);
    _session->close();
}

int
server::broadcast (session_list &_list, out_buffer_ptr _buf)
{
    arg_check(_buf);
    assert(!m_stopped);

    if (!m_stopping)
    {
        local_lock _lock(m_mutex);
        std::for_each(_list.begin(), _list.end(), [&_buf] (session_ptr &_s) {
            _s->send(_buf->clone());
        });
    }
    return 0;
}

void
server::parse_addr (const std::string &_name, uint16_t _port)
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
server::on_new_session (socket &&_sock)
{
    assert(!m_stopped);
    m_listener->check_thread();
    // FIXME: stop accepting when sessions reach to the maximum number

    address _local_addr = _sock.localaddr();
    address _peer_addr = _sock.peeraddr();

    session_ptr _session = nullptr;
    event_loop &_next_loop = assign_thread();
    try {
        _session = std::make_shared<session>(_next_loop,
                                             std::move(_sock),
                                             _local_addr, _peer_addr);
        _next_loop.register_event(_session);
    } catch (const std::exception &_e) {
        log_error("caught an exception when accepting new session, %s", _e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception when accepting new session");
        throw;
    }
    _session->set_keepalive(m_opts.keep_alive);
    _session->set_message_handler(m_message_handler);
    _session->set_sent_handler(m_sent_handler);
    _session->set_oob_handler(m_oob_handler);
    _session->set_close_handler(std::bind(&server::on_session_close, this, 
                                std::placeholders::_1, std::placeholders::_2));

    {
        local_lock _lock(m_mutex);
        m_sessions[_session->key()] = _session;
    }

    if (m_session_handler)
        _next_loop.run_in_loop([this, _session] () {
            log_excp_error(
                m_session_handler(_session),
                "server::m_session_handler() error"
            );
        });
}

void
server::on_session_close (const session &_session, std::error_code _ec)
{
    assert(!m_stopped);
    _session.check_thread();
    if (m_close_handler)
        log_excp_error(
            m_close_handler(std::cref(_session), _ec),
            "sever::m_close_handler() error"
        );

    if (!m_stopping)
    {
        local_lock _lock(m_mutex);
        m_sessions.erase(_session.key());
        log_debug("size = %lld", m_sessions.size());
    }
}

void
server::on_listener_close (std::error_code _ec)
{
    assert(!m_stopped);
    if (_ec)
        log_error("listener closed, %s",
                  system_error_str(_ec).c_str());
    stop();
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

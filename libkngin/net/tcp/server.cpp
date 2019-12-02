#ifdef _WIN32
#else
#include <netdb.h>
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
      m_stopped(true),
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

server::~server ()
{
    if (!m_stopped)
        stop();
}

bool
server::run ()
{
    check(m_stopped);

    // run threadpool
    if (!m_threadpool.start())
        return false;

    // create listen socket
    socket _sock(m_opts.allow_ipv6 ? socket::IPV6_TCP : socket::IPV4_TCP);
#warning "catch"

    // start listener
    event_loop_ptr _next_loop = m_opts.separate_listen_thread
                                ? m_threadpool.get_loop(0)
                                : m_threadpool.next_loop();
    check(_next_loop);
    m_listener = std::make_shared<listener>(_next_loop, std::move(_sock));

    // init address
    if (!parse_addr(m_opts.name, m_opts.port))
        return false;

    // bind
    m_listener->bind(m_listen_addr);

    // listen
    m_listener->listen(m_opts.backlog);

    m_stopped = false;
    log_info("TCP server is running");
    return true;
}

void
server::stop ()
{
    check(!m_stopped);
    log_info("stopping TCP server");

    m_listener->close([] (std::error_code _ec) {});
#warning "handle"
    thread::sleep(1000);
    {
        local_lock _lock(m_mutex);
        //for (auto _iter : m_sessions)
        //            log_debug("%s:%d, connected = %d",
        //            _iter.second->local_addr().addrstr().c_str(),
        //            _iter.second->local_addr().port(),
        //            _iter.second->connected());
        for (auto _iter : m_sessions)
            _iter.second->close();
    }
    thread::sleep(1000);
#warning "fix it"
    if (!m_stopped)
        m_threadpool.stop();
    m_stopped = true;

    log_info("TCP server has stopped");
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
    check(!m_stopped);
    check(_session->connected());

    _session->close();
}

int
server::broadcast (session_list &_list, out_buffer_ptr _buf)
{
    check(!m_stopped);
    return 0;
}

bool
server::parse_addr (const std::string &_name, uint16_t _port)
{
    addrinfo   _ai;
    addrinfo * _ai_list;
    ::bzero(&_ai, sizeof(addrinfo));
    _ai.ai_flags = AI_PASSIVE;
    _ai.ai_family = AF_UNSPEC;
    _ai.ai_protocol = 0;
    int _ret = ::getaddrinfo(_name.c_str(), std::to_string(_port).c_str(), &_ai, &_ai_list);
    if (_ret) {
        log_error("getaddrinfo() error - %s:%d", strerror(errno), errno);
        return false;
    }
    if (!_ai_list) {
        log_error("invalid name or port");
        freeaddrinfo(_ai_list);
        return false;
    }
    m_listen_addr = *(_ai_list->ai_addr);
    //addrinfo *_res = _ai_list;
    //while (_res) {
    //    ::getnameinfo(_res->ai_addr, _res->ai_addrlen, nullptr, 0, nullptr, 0, 0);
    //    _res = _res->ai_next;
    //}

    freeaddrinfo(_ai_list);
    return true;
}

void
server::on_new_session (socket &&_sock)
{
    check(!m_stopped);
    m_listener->check_thread();
    // FIXME: stop accepting when sessions reach to the maximum number

    address _local_addr = _sock.localaddr();
    address _peer_addr = _sock.peeraddr();

    session_ptr _session = nullptr;
    event_loop_ptr _next_loop = assign_thread();
    check(_next_loop);
    try {
        _session = std::make_shared<session>(_next_loop,
                                             std::move(_sock),
                                             _local_addr, _peer_addr);
    } catch (const k::exception &_e) {
        log_error("caught an exception when accepting new session: \"%s\"", _e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception when accepting new session");
        throw;
    }
    _session->set_message_handler(m_message_handler);
    _session->set_sent_handler(m_sent_handler);
    _session->set_oob_handler(m_oob_handler);
    _session->set_keepalive(m_opts.keep_alive);
    _session->set_close_handler([this] (const session &_session) {
        on_close(_session);
    });

    {
        local_lock _lock(m_mutex);
        m_sessions[_session->serial()] = _session;
    }

    if (m_session_handler)
        _next_loop->run_in_loop([this, _session] () {
            m_session_handler(_session);
        });
}

void
server::on_close (const session &_session)
{
    _session.check_thread();
    if (m_close_handler)
        m_close_handler(std::ref(_session));

    {
        local_lock _lock(m_mutex);
        m_sessions.erase(_session.serial());
    }
}

void
server::on_listener_error (listener &_listener)
{
    _listener.check_thread();
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

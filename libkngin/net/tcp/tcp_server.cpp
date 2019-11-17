#ifdef _WIN32
#else
#include <netdb.h>
#endif
#include <socket.h>
#include "tcp_server.h"
#include "common.h"

__NAMESPACE_BEGIN

tcp_server::tcp_server (const tcp_server_opts &_opts)
    try 
    : m_opts(_opts),
      m_threadpool(_opts.thread_num),
      m_connections(),
      m_listener(nullptr),
      m_listen_addr(),
      m_write_done_cb(nullptr),
      m_read_done_cb(nullptr),
      m_oob_cb(nullptr),
      m_close_cb(nullptr),
      m_stopped(true),
      m_mutex()
{
    if (_opts.disable_debug)
        logger()[0].disable_debug();
    if (_opts.disable_info)
        logger()[0].disable_info();
} catch (...) {
    log_fatal("tcp_server::tcp_server() error");
    throw;
}

tcp_server::~tcp_server ()
{
    if (!m_stopped)
        stop();
}

bool
tcp_server::run ()
{
    check(m_stopped);

    // run threadpool
    m_threadpool.start();

    // create listen socket
    socket _sock(m_opts.allow_ipv6 ? socket::IPV6_TCP : socket::IPV4_TCP);

    // start listener
    event_loop *_next_loop = m_opts.separate_listen_thread
                             ? m_threadpool.get_loop(0).get()
                             : m_threadpool.next_loop().get();
    m_listener = std::make_shared<listener>(_next_loop, std::move(_sock));
    m_listener->set_accept_cb(std::bind(&tcp_server::on_new_connection, this, std::placeholders::_1));
    m_listener->set_error_cb(std::bind(&tcp_server::on_listener_error, this, std::ref(*m_listener)));

    // init address
    if (!parse_addr(m_opts.name, m_opts.port))
        return false;

    // bind
    if (!m_listener->bind(m_listen_addr))
        return false;

    // listen
    if (!m_listener->listen(m_opts.backlog))
        return false;

    m_stopped = false;

    log_info("TCP server is running");
    return true;
}

void
tcp_server::stop ()
{
    check(!m_stopped);
    log_info("stopping TCP server");

    {
        local_lock _lock(m_mutex);
        for (auto _iter : m_connections)
            _iter.second->close();
    }
    m_listener->close();
    thread::sleep(1000);
    if (!m_stopped)
        m_threadpool.stop();
    m_stopped = true;

    log_info("TCP server has stopped");
}

void
tcp_server::remove_connection (tcp_connection_ptr _conn)
{
    check(!m_stopped);

    if (_conn->connected())
        _conn->close();
    else
        on_close(_conn);
}

int
tcp_server::broadcast (tcp_connection_list &_list, buffer &&_buf)
{
    check(!m_stopped);
    return 0;
}

bool
tcp_server::parse_addr (const std::string &_name, uint16_t _port)
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
tcp_server::on_new_connection (socket &&_sock)
{
    check(!m_stopped);

    // FIXME: stop accepting when connections reach to the maximum number

    address _local_addr, _peer_addr;
    _sock.localaddr(_local_addr);
    _sock.peeraddr(_peer_addr);

    tcp_connection_ptr _conn = nullptr;
    try {
        _conn = std::make_shared<tcp_connection>(assign_thread().get(), std::move(_sock),
                                                 _local_addr, _peer_addr);
    } catch (const k::exception &_e) {
        log_error("caught an exception when accepting new connection: \"%s\"", _e.what().c_str());
        log_dump(_e.dump());
    } catch (const std::exception &_e) {
        log_fatal("caught an std::exception when accepting new connection: \"%s\"", _e.what());
    } catch (...) {
        log_fatal("caught an undefined exception when accepting new connection");
    }
    _conn->set_read_done_cb(m_read_done_cb);
    _conn->set_write_done_cb(m_write_done_cb);
    _conn->set_oob_cb(m_oob_cb);
    _conn->set_close_cb(m_close_cb);

    {
        local_lock _lock(m_mutex);
        //log_debug("new[%d] - [localhost - %s:%d]",
        //          _conn->serial(), _peer_addr.addrstr().c_str(), _peer_addr.port());
        //for (auto _iter : m_connections)
        //    log_debug("[%d] - [localhost - %s:%d]",
        //            _iter.first,
        //            _iter.second->peer_addr().addrstr().c_str(),
        //            _iter.second->peer_addr().port());
        m_connections[_conn->serial()] = _conn;
    }

    if (m_connection_establish_cb)
        m_connection_establish_cb(_conn);
}

void
tcp_server::on_close (tcp_connection_ptr _conn)
{
    if (m_close_cb)
        m_close_cb(std::ref(*_conn));

    {
        local_lock _lock(m_mutex);
        m_connections.erase(_conn->serial());
    }
}

void
tcp_server::on_listener_error (listener &_listener)
{

}

__NAMESPACE_END

// signal ergent
// adjust size of epoll_event_set

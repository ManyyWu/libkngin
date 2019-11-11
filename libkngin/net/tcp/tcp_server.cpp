#include "define.h"
#include "tcp_server.h"
#include "common.h"
#include "logfile.h"

__NAMESPACE_BEGIN

tcp_server::tcp_server (address &&_listen_addr, const tcp_server_opts &_opts)
    try 
    : m_opts(_opts),
      m_threadpool(_opts.thread_num),
      m_connections(),
      m_listener(nullptr),
      m_listen_addr(std::move(_listen_addr)),
      m_new_connection_cb(nullptr),
      m_write_done_cb(nullptr),
      m_read_done_cb(nullptr),
      m_oob_cb(nullptr),
      m_close_cb(nullptr),
      m_stopped(true),
      m_mutex()
{
} catch (...) {
    log_fatal("tcp_server::tcp_server() error");
    throw;
}

tcp_server::~tcp_server ()
{
    if (!m_connections.empty())
        log_warning("there are still have %" PRIu64 " connections not been removed in tcp_server", m_fd_set.size());
    if (m_listener->connected())
        m_listener->close();
    if (!m_stopped)
        m_threadpool.stop();
}

void
tcp_server::run ()
{
    check(m_stopped);

    log_info("TCP server is running");
}

void
tcp_server::stop ()
{
    check(!m_stopped);

    log_info("TCP server has stopped");
}

bool
tcp_server::remove_connection (tcp_connection &_conn)
{
    check(!m_stopped);

    if (_conn.connected())
        _conn.stop();
    else
        on_close(_conn);
}

int
tcp_server::broadcast (tcp_connection_list &_list, buffer &&_buf)
{
    check(!m_stopped);
}

void
tcp_server::on_new_connection (socket &&_sock)
{
    check(_sock);
    check(!m_stopped);
    
    // FIXME: stop accepting when connections reach to the maximum number

    // create a new connection
    address _local_addr, _peer_addr;
    _sock.localaddr(_local_addr);
    _sock.peeraddr(_peer_addr);
    tcp_connection *_conn = nullptr;
    try {
        address _local_addr, _peer_addr;
        _sock.localaddr(_local_addr);
        _sock.peeraddr(_peer_addr);
        _conn = new tcp_connection(assign_thread(), std::move(socket), _local_addr, _peer_addr);
        _conn->set_read_done_cb(m_read_done_cb);
        _conn->set_write_done_cb(m_write_done_cb);
        _conn->set_oob_cb(m_oob_cb);
        _conn->set_close_cb(m_close_cb);

        {
            local_lock _lock(m_mutex);
            m_connections.insert[];
        }
    } catch (...) {
        _conn = nullptr;
        throw;
    }

    if (m_new_connection_cb)
        m_new_connection_cb(std::move(_sock));
}

void
tcp_server::on_close (tcp_connection &_conn)
{
    if (m_close_cb)
        m_close_cb(_conn);

    {
        local_lock _lock(m_mutex);
        m_connections.erase(_conn.socket.fd());
    }
}

__NAMESPACE_END

// signal ergent
// adjust size of epoll_event_set
// wait for looping
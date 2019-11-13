#include "define.h"
#include "tcp_connection.h"
#include "common.h"
#include "logfile.h"
#include "socket.h"
#include "error.h"
#include "buffer.h"
#include "net_buffer.h"
#include "epoller_event.h"
#include "epoller.h"
#include "lock.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/tcp_connection.cpp"

__NAMESPACE_BEGIN

tcp_connection::tcp_connection (event_loop *_loop, k::socket &&_socket,
                                const address &_local_addr, const address &_peer_addr)
    try
    : m_loop(_loop), 
      m_socket(std::move(_socket)), 
      m_event(_loop, &m_socket),
      m_connected(true), 
      m_local_addr(_local_addr), 
      m_peer_addr(_peer_addr),
      m_write_done_cb(nullptr), 
      m_read_done_cb(nullptr), 
      m_oob_cb(nullptr), 
      m_close_cb(nullptr),
      m_out_buf(), 
      m_in_buf(nullptr)
{
    check(_loop);
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    sockopts::set_ooblinline(m_socket, false);
    m_event.set_read_cb(std::bind(&tcp_connection::on_read, this));
    m_event.set_write_cb(std::bind(&tcp_connection::on_write, this));
    m_event.set_error_cb(std::bind(&tcp_connection::on_error, this));
    m_event.set_close_cb(std::bind(&tcp_connection::on_close, this));
    m_event.set_oob_cb(std::bind(&tcp_connection::on_oob, this));
    m_event.disable_write();
    m_event.disable_read();
    m_event.disable_oob();
    m_event.start();

    log_debug("new connection [%s:%d-%s:%d]",
              m_local_addr.addrstr().c_str(), m_local_addr.port(),
              m_peer_addr.addrstr().c_str(), m_peer_addr.port());
} catch (...) {
    log_fatal("tcp_connection::tcp_connection() error");
    throw;
}

tcp_connection::~tcp_connection ()
{
    if (m_connected)
        log_error("the TCP connection must be closed before object disconstructing");

    // FIXME; wait for m_connected to be false
}

bool
tcp_connection::send (buffer &_buf)
{
    check(m_connected);
    if (m_out_buf.readable())
        return false;

    m_event.enable_write();
    m_event.update();
    m_out_buf.clear();
    m_out_buf.swap(_buf);
    if (m_loop->in_loop_thread())
        on_write();
    else
        m_loop->run_in_loop(std::bind(&tcp_connection::on_write, this));
    return true;
}

bool
tcp_connection::recv (buffer &_buf)
{
    check(m_connected);

    m_event.enable_read();
    m_event.update();

    m_in_buf = &_buf;
    if (m_loop->in_loop_thread())
        on_read();
    else
        m_loop->run_in_loop(std::bind(&tcp_connection::on_read, this));
    return true;
}

void
tcp_connection::close ()
{ 
    check(m_connected);
    if (m_loop->in_loop_thread())
        on_close();
    else
        m_loop->run_in_loop(std::bind(&tcp_connection::on_close, this));
}

void
tcp_connection::rd_shutdown ()
{
    check(m_connected);
    if (m_loop->in_loop_thread())
        m_socket.rd_shutdown();
    else
        m_loop->run_in_loop(std::bind(&socket::rd_shutdown, &m_socket));
}

void
tcp_connection::wr_shutdown ()
{
    check(m_connected);
    if (m_loop->in_loop_thread())
        m_socket.wr_shutdown();
    else
        m_loop->run_in_loop(std::bind(&socket::wr_shutdown, &m_socket));
}

void
tcp_connection::on_write ()
{
    check(m_connected);
    m_loop->check_thread();

    size_t _readable_bytes = m_out_buf.readable();
    if (!m_event.pollout() || !_readable_bytes)
        return;

    ssize_t _size = m_socket.write(m_out_buf, _readable_bytes);
    if (_size > 0) {
        if (m_out_buf.readable())
            return;

        // done
        m_event.disable_write();
        m_event.update();
        m_out_buf.clear();
        if (m_write_done_cb)
            m_write_done_cb(std::ref(*this));
    } else {
        if ((EWOULDBLOCK == errno || EAGAIN == errno) && m_socket.nonblock())
            return;
        log_error("socket::write() error - %s:%d", strerror(errno), errno);
        on_error();
        return;
    }
}

void
tcp_connection::on_read ()
{
    check(m_connected);
    check(m_in_buf);
    m_loop->check_thread();

    size_t _writeable_bytes = m_in_buf->writeable();
    if (!m_event.pollin() || !_writeable_bytes)
        return;

    ssize_t _size = m_socket.read(*m_in_buf, _writeable_bytes);
    if (_size > 0) {
        if (m_in_buf->writeable())
            return;

        // done
        m_event.disable_read();
        m_event.update();
        if (m_read_done_cb)
            m_read_done_cb(std::ref(*this), *m_in_buf, m_in_buf->readable());
        m_in_buf = nullptr;
    } else {
        if ((EWOULDBLOCK == errno || EAGAIN == errno) && m_socket.nonblock())
            return;
        log_error("socket::write() error - %s:%d", strerror(errno), errno);
        on_error();
        return;
    }
}

void
tcp_connection::on_close ()
{
    check(m_connected);
    m_loop->check_thread();

    m_event.remove();
    m_socket.close();
    m_in_buf = nullptr;
    m_connected = false;
    
    log_debug("connection [%s:%d-%s:%d] closed",
              m_local_addr.addrstr().c_str(), m_local_addr.port(),
              m_peer_addr.addrstr().c_str(), m_peer_addr.port());

    if (m_close_cb)
        m_close_cb(std::ref(*this));
}

void
tcp_connection::on_oob ()
{
    check(m_connected);
    m_loop->check_thread();

    //recv
    buffer _buf(1);
    ssize_t _size = m_socket.recv(_buf, 1, MSG_OOB);
    if (1 != _size) {
        log_error("socket::recv(MSG_OOB) error - %s:%d", strerror(errno), errno);
        on_error();
        return;
    }
    if (m_oob_cb)
        m_oob_cb(std::ref(*this), _buf.read_uint8());
    else {
        log_warning("unhandled oob data from %s:%hu",
                    m_local_addr.addrstr().c_str(), m_local_addr.port());
    }
}

void
tcp_connection::on_error()
{
    check(m_connected);
    m_loop->check_thread();

    int _err_code = 0;
    if (!sockopts::error(m_socket, _err_code))
        log_error("sockopts::error() error");
    else
        log_error("handled an socket error, fd = %d - %s:%d", m_socket.fd(), strerror(errno), errno);
    on_close();
}

__NAMESPACE_END

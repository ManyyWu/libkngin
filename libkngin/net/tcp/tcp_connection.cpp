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
#include "local_lock.h"

__NAMESPACE_BEGIN

tcp_connection::tcp_connection (event_loop *_loop, k::socket &&_socket,
                                address &&_local_addr, address &&_peer_addr)
    : m_loop(_loop), m_socket(std::move(_socket)), m_event(_loop, &m_socket),
      m_connected(true), m_local_addr(_local_addr), m_peer_addr(_peer_addr),
      m_write_done_cb(nullptr), m_read_done_cb(nullptr), m_oob_cb(nullptr), m_close_cb(nullptr),
      m_out_buf(), m_in_buf(NULL)
{
    check(_loop);
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    sockopts::set_ooblinline(m_socket, false);
    m_event.set_read_cb(std::bind(&tcp_connection::handle_read, this));
    m_event.set_write_cb(std::bind(&tcp_connection::handle_write, this));
    m_event.set_error_cb(std::bind(&tcp_connection::handle_error, this));
    m_event.set_close_cb(std::bind(&tcp_connection::handle_close, this));
    m_event.set_oob_cb(std::bind(&tcp_connection::handle_oob, this));
    m_event.start();
}

tcp_connection::~tcp_connection ()
{
}

bool
tcp_connection::send (buffer &&_buf)
{
    check(&_buf);
    check(m_connected);
    if (m_out_buf.readable())
        return false;

    m_out_buf.swap(_buf);
    if (m_loop->in_loop_thread()) {
        handle_read();
    } else {
        m_loop->run_in_loop(std::bind(&tcp_connection::handle_write, this));
    }
    m_event.enable_write();
    m_event.update();
    return true;
}

bool
tcp_connection::recv (buffer &_buf)
{
    check(&_buf);
    check(m_connected);

    m_in_buf = &_buf;
    if (m_loop->in_loop_thread()) {
        handle_read();
    } else {
        m_loop->run_in_loop(std::bind(&tcp_connection::handle_read, this));
    }
    m_event.enable_read();
    m_event.update();
    return true;
}

void
tcp_connection::close ()
{ 
    check(m_connected);
    m_loop->run_in_loop(std::bind(&socket::close, &m_socket));
}

void
tcp_connection::rd_shutdown ()
{
    check(m_connected);
    m_loop->run_in_loop(std::bind(&socket::rd_shutdown, &m_socket));
}

void
tcp_connection::wr_shutdown ()
{
    check(m_connected);
    m_loop->run_in_loop(std::bind(&socket::wr_shutdown, &m_socket));
}

void
tcp_connection::handle_write ()
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
            m_write_done_cb(*this);
    } else {
        if ((EWOULDBLOCK == _size || EAGAIN == _size) && m_socket.nonblock())
            return;
        log_error("socket::write() error - %s:%d", strerror(errno), errno);
        handle_error();
        return;
    }
}

void
tcp_connection::handle_read ()
{
    check(m_connected);
    check(m_in_buf);
    m_loop->check_thread();

    size_t _writeable_bytes = m_in_buf->writeable();
    if (!m_event.pollout() || !_writeable_bytes)
        return;

    ssize_t _size = m_socket.read(*m_in_buf, _writeable_bytes);
    if (_size > 0) {
        if (m_in_buf->writeable())
            return;

        // done
        m_event.disable_read();
        m_event.update();
        m_in_buf->clear();
        if (m_read_done_cb)
            m_read_done_cb(*this, *m_in_buf, m_in_buf->readable());
    } else {
        if ((EWOULDBLOCK == _size || EAGAIN == _size) && m_socket.nonblock())
            return;
        log_error("socket::write() error - %s:%d", strerror(errno), errno);
        handle_error();
        return;
    }
}

void
tcp_connection::handle_close ()
{
    check(m_connected);
    m_loop->check_thread();

    m_connected = false; 
    m_event.set_flags(0);
    m_event.update();
//    m_socket.close();
    if (m_close_cb)
        m_close_cb(*this);
}

void
tcp_connection::handle_oob ()
{
    check(m_connected);
    m_loop->check_thread();

    //recv
    buffer _buf(1);
    ssize_t _size = m_socket.recv(_buf, 1, MSG_OOB);
    if (1 != _size) {
        log_error("socket::recv(MSG_OOB) error - %s:%d", strerror(errno), errno);
        handle_error();
        return;
    }
    if (m_oob_cb)
        m_oob_cb(*this, _buf.read_uint8());
    else {
        inet_addrstr _addrstr;
        log_warning("unhandled oob data from %s:%hu", m_local_addr.addrstr(_addrstr), m_local_addr.port());
    }
}

void
tcp_connection::handle_error()
{
    check(m_connected);
    m_loop->check_thread();

    int _err_code = 0;
    sockopts::error(m_socket, _err_code);
    log_error("handled an socket error, fd = %d - %s:%d", m_socket.fd(), strerror(errno), errno);
    m_loop->run_in_loop(std::bind(&tcp_connection::handle_close, this));
}

__NAMESPACE_END

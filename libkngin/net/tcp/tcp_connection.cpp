#include "tcp_connection.h"
#include "common.h"
#include "socket.h"
#include "buffer.h"
#include "net_buffer.h"
#include "epoller_event.h"
#include "epoller.h"
#include "lock.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/tcp_connection.cpp"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

uint64_t tcp_connection::m_next_serial = 0;

tcp_connection::tcp_connection (event_loop *_loop, k::socket &&_socket,
                                const address &_local_addr, const address &_peer_addr)
    try
    : m_loop(_loop), 
      m_socket(std::move(_socket)), 
      m_event(_loop, &m_socket),
      m_connected(true), 
      m_local_addr(_local_addr), 
      m_peer_addr(_peer_addr),
      m_sent_cb(nullptr),
      m_message_cb(nullptr),
      m_oob_cb(nullptr), 
      m_close_cb(nullptr),
      m_out_bufq(),
      m_in_buf(nullptr),
      m_serial(tcp_connection::next_serial())
{
    check(_loop);
    if (!m_socket.set_closeexec(true)) {
        log_error("socket::set_closeexec(true) error");
        throw k::exception("socket::set_closeexec() error");
    }
    if (!m_socket.set_nonblock(true)) {
        log_error("socket::set_nonblock(true) error");
        throw k::exception("socket::set_nonblock() error");
    }
    if (!sockopts::set_ooblinline(m_socket, false)) {
        log_error("sockopts::set_ooblinline(false) error");
        throw k::exception("sockopts::set_ooblinline() error");
    }
    m_event.set_read_cb(std::bind(&tcp_connection::on_read, this));
    m_event.set_write_cb(std::bind(&tcp_connection::on_write, this));
    m_event.set_error_cb(std::bind(&tcp_connection::on_error, this));
    m_event.set_close_cb(std::bind(&tcp_connection::on_close, this));
    m_event.set_oob_cb(std::bind(&tcp_connection::on_oob, this));
    m_event.disable_write();
    m_event.disable_read();
    m_event.disable_oob();
    m_event.start();
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
tcp_connection::send (buffer_ptr _buf)
{
    check(m_connected);

    {
        local_lock _lock(m_mutex);
        m_out_bufq.push_front(_buf);
        m_event.enable_write();
        m_event.update();
    }

    if (m_loop->in_loop_thread())
        on_write();
    else
        m_loop->run_in_loop(std::bind(&tcp_connection::on_write, this));
    return true;
}

bool
tcp_connection::recv (buffer_ptr _buf)
{
    check(m_connected);

    m_event.enable_read();
    m_event.update();

    m_in_buf = _buf;
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
    if (!m_connected)
        return;
    m_loop->check_thread();

    buffer_ptr _buf = nullptr;
    {
        local_lock _lock(m_mutex);
        _buf = m_out_bufq.back();
    }

    size_t _readable_bytes = _buf->readable();
    check(_readable_bytes);
    if (!m_event.pollout())
        return;

    ssize_t _size = m_socket.write(*_buf, _readable_bytes);
    if (_size > 0) {
        if (_buf->readable())
            return;

        // write done
        {
            local_lock _lock(m_mutex);
            m_out_bufq.pop_back();
            if (m_out_bufq.empty()) {
                m_event.disable_write();
                m_event.update();
            }
        }
        if (m_sent_cb)
            m_sent_cb(std::ref(*this));
    } else if (!_size) {
        on_close();
        return;
    } else {
        if (((EWOULDBLOCK == errno || EAGAIN == errno) && m_socket.nonblock()) || EINTR == errno)
            return;
        log_error("socket::write() error - %s:%d", strerror(errno), errno);
        on_error();
        return;
    }
}

void
tcp_connection::on_read ()
{
    if (!m_connected)
        return;
    m_loop->check_thread();

    if (!m_in_buf)
        return;

    size_t _writeable_bytes = m_in_buf->writeable();
    check(_writeable_bytes);
    if (!m_event.pollin())
        return;

    ssize_t _size = m_socket.read(*m_in_buf, _writeable_bytes);
    if (_size > 0) {
        if (m_in_buf->writeable())
            return;

        // read done
        m_event.disable_read();
        m_event.update();
        buffer_ptr _temp_ptr = m_in_buf;
        m_in_buf = nullptr;
        if (m_message_cb)
            m_message_cb(std::ref(*this), *_temp_ptr, _temp_ptr->readable());
    } else if (!_size) {
        on_close();
        return;
    } else {
        if (((EWOULDBLOCK == errno || EAGAIN == errno) && m_socket.nonblock()) || EINTR == errno)
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
    if (m_oob_cb) {
        m_oob_cb(std::ref(*this), _buf.read_uint8());
    } else {
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

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

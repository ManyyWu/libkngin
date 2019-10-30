#include "define.h"
#include "connection.h"
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

connection::connection(event_loop *_loop, socket &&_socket,
                       const address &_local_addr, const address &_peer_addr)
    : m_loop(_loop), m_socket(std::move(_socket)), m_event(_loop, &m_socket),
      m_connected(true), m_local_addr(_local_addr), m_peer_addr(_peer_addr),
      m_writable_cb(nullptr), m_readable_cb(nullptr), m_write_done_cb(nullptr), 
      m_read_done_cb(nullptr), m_oob_cb(nullptr), m_close_cb(nullptr),
      m_out_buf(), m_in_buf(), m_mutex()
{
    check(_loop);
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    m_event.set_read_cb(std::bind(&connection::handle_read, this));
    m_event.set_write_cb(std::bind(&connection::handle_write, this));
    m_event.set_error_cb(std::bind(&connection::handle_error, this));
    m_event.set_close_cb(std::bind(&connection::handle_close, this));
    m_event.set_oob_cb(std::bind(&connection::handle_oob, this));
}

connection::~connection()
{
}

bool
connection::send (buffer &&_buf)
{
    check(m_connected);
    check(&_buf);

    {
        local_lock _lock(m_mutex);
        m_out_buf.list.push_back(buffer(std::move(_buf)));
        m_out_buf.list.back().reset();
    }
    if (m_loop->in_loop_thread()) {
        // write
    } else {
        if (!m_loop->run_in_loop(std::bind(&connection::handle_write, this))) {
            log_error("event_loop::run_in_loop() error");
            return false;
        }
    }
    return true;
}

bool
connection::recv (buffer &&_buf, size_t _size)
{
    check(_buf.size() >= _size);
    check(m_connected);
    check(&_buf);

    {
        local_lock _lock(m_mutex);
        m_in_buf.push_back(buffer(std::move(_buf)));
        m_in_buf.back().reset();
    }
    if (m_loop->in_loop_thread()) {
        // read
    } else {
        if (!m_loop->run_in_loop(std::bind(&connection::handle_read, this))) {
            log_error("event_loop::run_in_loop() error");
            return false;
        }
    }
    return true;
}

void
connection::set_read_done_cb (read_done_cb &&_cb)  
{ 
    m_read_done_cb = std::move(_cb); 
}

void
connection::set_write_done_cb (write_done_cb &&_cb) 
{ 
    m_write_done_cb = std::move(_cb); 
}

void
connection::set_close_cb (close_cb &&_cb) 
{ 
    m_close_cb = std::move(_cb); 
}

void
connection::set_oob_cb (read_oob_cb &&_cb) 
{ 
    m_oob_cb = std::move(_cb); 
}

void
connection::handle_write ()
{
    check(m_connected);
    m_loop->check_thread();
    if (!m_out_buf.readable())
        return;

    buffer _buf;
    {
        local_lock _lock(m_mutex);
        _buf.swap(m_out_buf.list().front());
        m_out_buf.list().pop_front();
    }
    size_t _buf_size = _buf.readable();
    ssize_t _size = m_socket.write(_buf, _buf_size);
    if (_size > 0) {
        if (_buf_size != _size)
            return;

        // done
        m_event.disable_write();
        if (m_write_done_cb)
            m_write_done_cb(*this);
    } else {
        if ((EWOULDBLOCK == _size || EAGAIN == _size) && m_socket.nonblock())
            return;
        log_error("socket::write() error - %s:%d", strerror(errno), errno);
    }
}

void
connection::handle_read ()
{
    check(m_connected);
    m_loop->check_thread();
}

void
connection::handle_close ()
{
    check(m_connected);
    m_connected = false;
    m_loop->check_thread();

    m_socket.close();
    if (m_close_cb)
        m_close_cb(*this);

    m_out_buf.clear();
    m_in_buf.clear();
}

void
connection::handle_oob ()
{
    check(m_connected);
    m_loop->check_thread();

    //recv
    //run in loop
}

void
connection::handle_error()
{
    check(m_connected);
    m_loop->check_thread();

    int _err_code = 0;
    sockopts::error(m_socket, _err_code);
    log_error("handled an socket error, fd = %d - %s:%d", m_socket.fd(), strerror(errno), errno);
    if (m_loop->run_in_loop(std::bind(&connection::handle_close, this)))
        log_error("event_loop::run_in_loop() error");
}

__NAMESPACE_END

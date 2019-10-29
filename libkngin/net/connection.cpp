#include "define.h"
#include "connection.h"
#include "common.h"
#include "logfile.h"
#include "socket.h"
#include "error.h"
#include "buffer.h"
#include "epoller_event.h"
#include "epoller.h"

__NAMESPACE_BEGIN

connection::connection(k::event_loop *_loop, k::socket &&_socket,
                       const address &_local_addr, const address &_peer_addr)
    : m_loop(_loop), m_socket(std::move(_socket)), m_event(_loop, &m_socket),
      m_connected(true), m_local_addr(_local_addr), m_peer_addr(_peer_addr),
      m_writable_cb(nullptr), m_readable_cb(nullptr), m_write_done_cb(nullptr), 
      m_read_done_cb(nullptr), m_oob_cb(nullptr), m_close_cb(nullptr),
      m_out_buf(NULL), m_out_size(0), m_in_buf(NULL), m_in_size(0)
{
    check(_loop);
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    m_event.enable_once();
}

connection::~connection()
{
}

bool
connection::send (buffer &_buf, size_t _size)
{
    check(_buf.size() >= _size);
    check(m_out_buf = &_buf);
    check(m_connected);

    if (m_loop->in_loop_thread()) {
        // write
    } else {
        if (!m_loop->run_in_loop(std::bind(&connection::handle_write, this))) {
            log_error("event_loop::run_in_loop() error");
            return false;
        }
        m_event.update();
    }
    return true;
}

bool
connection::recv (buffer &_buf, size_t _size)
{
    check(_buf.size() >= _size);
    check(m_in_buf = &_buf);
    check(m_connected);

    if (m_loop->in_loop_thread()) {
        // read
    } else {
        if (!m_loop->run_in_loop(std::bind(&connection::handle_read, this))) {
            log_error("event_loop::run_in_loop() error");
            return false;
        }
        m_event.update();
    }
    return true;
}

void
connection::set_read_done_cb (read_done_cb &&_cb)  
{ 
    m_read_done_cb = std::move(_cb); 
    m_event.set_read_cb(std::bind(&connection::handle_read, this); 
    m_event.update();
}

void
connection::set_write_done_cb (write_done_cb &&_cb) 
{ 
    m_write_done_cb = std::move(_cb); 
    m_event.set_write_cb(std::bind(&connection::handle_write, this); 
    m_event.update();
}

void
connection::set_close_cb (close_cb &&_cb) 
{ 
    m_close_cb = std::move(_cb); 
    m_event.set_close_cb(std::bind(&connection::handle_close, this); 
    m_event.update();
}

void
connection::set_oob_cb (read_oob_cb &&_cb) 
{ 
    m_oob_cb = std::move(_cb); 
    m_event.set_oob_cb(std::bind(&connection::handle_oob, this); 
    m_event.update();
}

void
connection::handle_write ()
{
    check(m_out_buf);
    check(m_connected);
    m_loop->check_thread();

    ssize_t _size = m_socket.write(*m_out_buf, m_out_size);
    if (_size > 0) {
        if (m_out_size != m_out_buf->writeable())
            return;
        if (m_write_done_cb)
            if (!m_loop->run_in_loop(std::bind(m_write_done_cb, *this/*, *m_out_buf, _size*/)))
                log_error("event_loop::run_in_loop() error");
    } else {
        if ((EWOULDBLOCK == _size || EAGAIN == _size) && m_socket.nonblock())
            return;
        log_error("socket::write() error - %s:%d", strerror(errno), errno);
    }

    m_out_buf = NULL;
    m_out_size = 0;
//    m_event.disable_write();
//    m_event.update();
}

void
connection::handle_read ()
{
    check(m_in_buf);
    check(m_connected);
    m_loop->check_thread();

    m_in_buf = NULL;
    m_in_size = 0;
//    m_event.disable_read();
//    m_event.update();
}

void
connection::handle_close ()
{
    check(m_connected);
    m_connected = true;
    m_loop->check_thread();

    m_socket.close();
    if (m_close_cb)
        m_close_cb(*this);

    m_out_buf = NULL;
    m_in_buf = NULL;
    m_out_size = 0;
    m_in_size = 0;
//    m_event.disable_close();
//    m_event.update();
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

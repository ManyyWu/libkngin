#include "define.h"
#include "connection.h"
#include "common.h"
#include "logfile.h"
#include "socket.h"
#include "buffer.h"
#include "epoller_event.h"
#include "epoller.h"

__NAMESPACE_BEGIN

connection::connection(k::event_loop *_loop, k::socket &&_socket,
                       const address &_local_addr, const address &_peer_addr)
    : m_loop(_loop), m_socket(std::move(_socket)), m_event(_loop, &m_socket),
      m_connected(true), m_local_addr(_local_addr), m_peer_addr(_peer_addr),
      m_writable_cb(nullptr), m_readable_cb(nullptr),
      m_write_done_cb(nullptr), m_read_done_cb(nullptr)
{
    kassert(_loop);
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
}

connection::~connection()
{
}

bool
connection::recv (buffer &_buf)
{
}

bool
connection::send (const buffer &_buf)
{
}

void
connection::close ()
{
}

void
connection::handle_write ()
{
    kassert(m_out_buf);
    m_loop->check_thread();
    ssize_t _ret = m_socket.write(*m_out_buf, m_out_buf->size());
    if (_ret > 0) {
        if (m_write_done_cb) {
            m_out_buf->rreset(0);
            m_write_done_cb(*this, m_out_buf);
        }
    } else if (0 == _ret) {
    } else { // error
        // log
    }
}

void
connection::handle_read ()
{
    m_loop->check_thread();
}

void
connection::handle_error ()
{
    m_loop->check_thread();
}

void
connection::handle_close ()
{
    m_loop->check_thread();
}

void
connection::run_in_loop (event_loop::queued_fn &&_fn)
{
}

__NAMESPACE_END

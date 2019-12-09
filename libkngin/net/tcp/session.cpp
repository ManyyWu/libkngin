#include "core/common.h"
#include "core/lock.h"
#include "core/buffer.h"
#include "core/system_error.h"
#include "net/socket.h"
#include "net/epoller_event.h"
#include "net/epoller.h"
#include "net/tcp/session.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/session.cpp"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

uint64_t session::m_next_serial = 0;

session::session (event_loop &_loop, k::socket &&_socket,
                  const address &_local_addr, const address &_peer_addr)
    try
    : epoller_event(_socket.fd()),
      m_loop(_loop.pimpl()),
      m_socket(std::move(_socket)), 
      m_connected(true),
      m_local_addr(_local_addr), 
      m_peer_addr(_peer_addr),
      m_name(m_socket.name()),
      m_sent_handler(nullptr),
      m_message_handler(nullptr),
      m_oob_handler(nullptr),
      m_close_handler(nullptr),
      m_out_bufq(),
      m_in_buf(nullptr),
      m_callback_lowat(KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT),
      m_serial(session::next_serial()),
      m_mutex()
{
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    sockopts::set_ooblinline(m_socket, false);
    enable_read();
    enable_write();
    enable_oob();
} catch (...) {
    log_fatal("session::session() error");
    throw;
}

session::~session () KNGIN_NOEXCP
{
    if (m_connected) {
        log_warning("the TCP session must be closed"
                    " before object disconstructing");
        ignore_excp(this->close(true));
    }

    // FIXME; wait for m_connected to be false( this->close(true); )
}

bool
session::send (out_buffer_ptr _buf)
{
    arg_check(_buf);
    assert(m_connected);

    {
        local_lock _lock(m_mutex);
        m_out_bufq.push_front(_buf);
        enable_write();
        m_loop->update_event(self());
    }

    if (m_loop->in_loop_thread())
        on_write();
    else
        m_loop->run_in_loop([this] () {
            on_write();
        });
    return true;
}

bool
session::recv (in_buffer_ptr _buf, size_t _lowat /* = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT */)
{
    arg_check(_buf);
    assert(m_connected);
    enable_read();
    m_loop->update_event(self());

    m_in_buf = _buf;
    m_callback_lowat = _lowat;
    if (m_loop->in_loop_thread())
        on_read();
    else
        m_loop->run_in_loop([this] () {
            on_read();
        });
    return true;
}

void
session::close (bool _blocking /* = false */)
{
    assert(m_connected);
    if (!m_loop->looping()) {
        m_socket.close();
        m_in_buf = nullptr;
        m_connected = false;
        return;
    }
    m_loop->remove_event(self());
    if (m_loop->in_loop_thread()) {
        on_close(std::error_code());
    } else {
        if (_blocking) {
            std::shared_ptr<barrier> _barrier_ptr = std::make_shared<barrier>(2);
            m_loop->run_in_loop([this, _barrier_ptr] () {
                on_close(std::error_code());
                if (_barrier_ptr->wait())
                    _barrier_ptr->destroy();
            });
            if (_barrier_ptr->wait())
                _barrier_ptr->destroy();
        } else {
            m_loop->run_in_loop([this] () {
                on_close(std::error_code());
            });
        }
    }
}

void
session::rd_shutdown ()
{
    assert(m_connected);
    if (m_loop->in_loop_thread())
        m_socket.rd_shutdown();
    else
        m_loop->run_in_loop([this] () {
            m_socket.rd_shutdown();
        });
}

void
session::wr_shutdown ()
{
    assert(m_connected);
    if (m_loop->in_loop_thread())
        m_socket.wr_shutdown();
    else
        m_loop->run_in_loop([this] () {
            m_socket.wr_shutdown();
        });
}

void
session::on_write ()
{
    if (!m_connected)
        return;
    if_not (pollout())
        return;
    m_loop->check_thread();

    out_buffer_ptr _buf = nullptr;
    {
        local_lock _lock(m_mutex);
        if (m_out_bufq.empty())
            return;
        _buf = m_out_bufq.back();
    }

    if_not (_buf->size())
        return;

    std::error_code _ec;
    size_t _size = m_socket.write(*_buf, _ec);
    if (_ec) {
        if (((std::errc::operation_would_block == _ec ||
              std::errc::resource_unavailable_try_again == _ec ||
              std::errc::interrupted == _ec
              ) && m_socket.nonblock()
             ) || EINTR == errno
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
        on_error();
#warning "error_code"
        return;
    }
    if (!_size) {
        on_close(_ec);
        return;
    } else {
        if (_buf->size())
            return;

        // write done
        {
            local_lock _lock(m_mutex);
            m_out_bufq.pop_back();
            if (m_out_bufq.empty()) {
                disable_write();
                m_loop->update_event(self());
            }
        }
        if (m_sent_handler)
            log_excp_error(
                m_sent_handler(std::ref(*this)),
                "session::m_sent_handler() error"
            );
    }
}

void
session::on_read ()
{
    if (!m_in_buf)
        return;
    if (!m_connected)
        return;
    if_not (pollin())
        return;

    m_loop->check_thread();

    size_t _writeable_bytes = m_in_buf->writeable();
    if_not (_writeable_bytes)
        return;

    std::error_code _ec;
    size_t _size = m_socket.read(*m_in_buf, _ec);
    if (_ec) {
        if (((std::errc::operation_would_block == _ec ||
              std::errc::resource_unavailable_try_again == _ec ||
              std::errc::interrupted == _ec
              ) && m_socket.nonblock()
             ) || EINTR == errno
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
        on_error();
#warning "error_code"
        return;
    }
    if (!_size) {
        on_close(_ec);
        return;
    } else {
        if (m_in_buf->writeable() && m_in_buf->valid() < m_callback_lowat)
            return;

        // read done
        disable_read();
        m_loop->update_event(self());
        in_buffer_ptr _temp_ptr = m_in_buf;
        m_in_buf = nullptr;
        if (m_message_handler)
            log_excp_error(
                m_message_handler(std::ref(*this),
                                  std::ref(*_temp_ptr),
                                  _temp_ptr->valid()),
                "session::m_message_handler() error"
            );
    }
}

void
session::on_oob ()
{
    if (!m_connected)
        return;
    if_not (pollpri())
        return;
    m_loop->check_thread();

    //recv
    char _data;
    in_buffer _buf(&_data, 1);
    std::error_code _ec;
    size_t _size = m_socket.recv(_buf, MSG_OOB, _ec);
    if (_ec) {
        log_error("socket::recv(MSG_OOB) error, %s",
                  system_error_str(_ec).c_str());
        on_error();
#warning "error_code"
        return;
    }
    if (m_oob_handler) {
        log_excp_error(
            m_oob_handler(std::ref(*this), _data),
            "session::m_oob_handler() error"
        );
    } else {
        log_warning("unhandled oob data from %s", m_socket.name().c_str());
    }
}

void
session::on_error ()
{
    assert(m_connected);
    m_loop->check_thread();

    char _arr[1];
    in_buffer _buf(_arr, 1);
    std::error_code _ec = m_socket.read_error();
    if (_ec) {
        if (((std::errc::operation_would_block == _ec ||
              std::errc::resource_unavailable_try_again == _ec ||
              std::errc::interrupted == _ec
              ) && m_socket.nonblock()
             ) || EINTR == errno
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
#warning "error_code"
        return;
    } else {
        log_error("session::on_error(), no any error was readed");
    }
    on_close(_ec);
}

void
session::on_close (std::error_code _ec)
{
    assert(m_connected);
    m_loop->check_thread();

    if (m_loop->registed(self()))
        m_loop->remove_event(self());
    m_socket.close();
    m_in_buf = nullptr;
    m_connected = false;
    if (m_close_handler)
        log_excp_error(
            m_close_handler(std::cref(*this), _ec),
            "listener::m_close_handler() error"
        );
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

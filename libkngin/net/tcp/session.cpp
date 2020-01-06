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

session::session (event_loop &_loop, k::socket &&_socket,
                  const address &_local_addr, const address &_peer_addr)
    try
    : epoller_event(_socket.fd(), epoller_event::EVENT_TYPE_FILE),
      m_loop(_loop.weak_self()),
      m_socket(std::move(_socket)), 
      m_closed(false),
      m_local_addr(_local_addr), 
      m_peer_addr(_peer_addr),
      m_name(m_socket.name()),
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
      m_message_handler(nullptr),
      m_sent_handler(nullptr),
#else
      m_message_handlerq(),
      m_sent_handlerq(),
#endif
      m_oob_handler(nullptr),
      m_close_handler(nullptr),
      m_out_bufq(),
      m_in_bufq(),
#if (ON != KNGIN_SESSION_NO_MUTEX)
      m_out_bufq_mutex(),
      m_in_bufq_mutex(),
#endif
      m_callback_lowat(KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT),
      m_key(m_peer_addr.key())
{
    arg_check(m_socket.valid());

    // set socket options
    sockopts::set_ooblinline(m_socket, false);

    // set socket flags
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);

    // set event flags
    enable_read();
    disable_write();
    disable_oob();
} catch (...) {
    log_fatal("session::session() error");
    throw;
}

session::~session () KNGIN_NOEXCP
{
    if (!m_closed or registed()) {
        log_warning("the TCP session must be closed"
                    " before object disconstructing");
        ignore_excp(this->close(true));
    }
    // FIXME; wait for m_closed to be true ( this->close(true); )
}

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
bool
session::send (msg_buffer _buf)
#else
bool
session::send (msg_buffer _buf, sent_handler &&_handler)
#endif
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closed)
        return false;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(_loop->in_loop_thread());
#endif
    assert(_buf.buffer().begin() and _buf.buffer().size());

    {
#if (ON != KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_out_bufq_mutex);
#endif
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        m_sent_handlerq.push_front(_handler);
#endif
        m_out_bufq.push_front(_buf);
        if (m_out_bufq.size() <= 1) {
            enable_write();
            _loop->update_event(*this);
        }
    }

#if (ON == KNGIN_SESSION_NO_MUTEX)
    on_write();
#else
    if (_loop->in_loop_thread())
        on_write();
    else
        _loop->run_in_loop([this] () {
            on_write();
        });
#endif
    return true;
}

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
bool
session::recv (in_buffer _buf, size_t _lowat /* = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT */)
#else
bool
session::recv (in_buffer _buf, message_handler &&_handler,
               size_t _lowat /* = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT */)
#endif
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closed)
        return false;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(_loop->in_loop_thread());
#endif
    assert(_buf.begin() and _buf.size());
    assert(_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
           ? _buf.size() >= _lowat : true);

    {
#if (ON != KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_in_bufq_mutex);
#endif
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        m_message_handlerq.push_front(_handler);
#endif
        m_in_bufq.push_front(_buf);
        m_callback_lowat = _lowat;
    }

#if (ON == KNGIN_SESSION_NO_MUTEX)
    on_read();
#else
    if (_loop->in_loop_thread())
        on_read();
    else
        _loop->run_in_loop([this] () {
            on_read();
        });
#endif
    return true;
}

void
session::close (bool _blocking /* = false */)
{
    if (m_closed)
        return;
    auto _loop = m_loop.lock();

    if (registed() and _loop and  _loop->looping()) {
#if (ON == KNGIN_SESSION_NO_MUTEX)
        on_close(std::error_code());
#else
        if (_loop->in_loop_thread()) {
            on_close(std::error_code());
        } else {
            _loop->remove_event(*this);
            if (_blocking) {
                auto _barrier_ptr = std::make_shared<barrier>(2);
                _loop->run_in_loop([this, _barrier_ptr] () {
                    on_close(std::error_code());
                    if (_barrier_ptr->wait())
                        _barrier_ptr->destroy();
                });
                if (_barrier_ptr->wait())
                    _barrier_ptr->destroy();
            } else {
                _loop->run_in_loop([this] () {
                    on_close(std::error_code());
                });
            }
        }
#endif
    } else {
        on_close(); // no callback
    }
}

void
session::rd_shutdown ()
{
    auto _loop = m_loop.lock();
    if (!_loop)
        return;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(_loop->in_loop_thread());
#endif
    assert(!m_closed);
    assert(!m_socket.rd_closed());
    if (_loop->in_loop_thread())
        m_socket.rd_shutdown();
    else
        _loop->run_in_loop([this] () {
            m_socket.rd_shutdown();
        });
}

void
session::wr_shutdown ()
{
    auto _loop = m_loop.lock();
    if (!_loop)
        return;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(_loop->in_loop_thread());
#endif
    assert(!m_closed);
    assert(!m_socket.wr_closed());
    if (_loop->in_loop_thread())
        m_socket.wr_shutdown();
    else
        _loop->run_in_loop([this] () {
            m_socket.wr_shutdown();
        });
}

void
session::on_write ()
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closed)
        return;
    if_not (pollout())
        return;
    assert(_loop->in_loop_thread());

    msg_buffer *_buf = nullptr;
    {
#if (ON != KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_out_bufq_mutex);
#endif
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        assert(m_out_bufq.size() == m_sent_handlerq.size());
#endif
        if (m_out_bufq.empty())
            return;
        _buf = &m_out_bufq.back();
    }
    assert(_buf and _buf->buffer().size());

    std::error_code _ec;
    size_t _size = m_socket.write(_buf->buffer(), _ec);
    if (_ec) {
        if (((std::errc::operation_would_block == _ec or
              std::errc::resource_unavailable_try_again == _ec or
              std::errc::interrupted == _ec
              ) and m_socket.nonblock()
             ) or EINTR == errno
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
        on_error();
        return;
    }
    if (!_size) {
        on_close(_ec);
        return;
    } else {
        if (_buf->buffer().size())
            return;
        _buf = nullptr;

        // write done
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        sent_handler _handler = nullptr;
#endif
        {
#if (ON != KNGIN_SESSION_NO_MUTEX)
            local_lock _lock(m_out_bufq_mutex);
#endif
            m_out_bufq.pop_back();
            if (m_out_bufq.empty()) {
                disable_write();
                _loop->update_event(*this);
            }
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
            _handler = m_sent_handlerq.back();
            m_sent_handlerq.pop_back();
#endif
        }
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
        if (m_sent_handler) {
            log_excp_error(
                m_sent_handler(std::ref(*this)),
                "session::m_sent_handler() error"
            );
        }
#else
        if (_handler) {
            log_excp_error(
                _handler(std::ref(*this)),
                "session::m_sent_handler() error"
            );
        }
#endif
    }
}

void
session::on_read ()
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closed)
        return;
    if_not (pollin())
        return;
    assert(_loop->in_loop_thread());
    if (m_in_bufq.empty()) {
        //log_debug("%" PRIu64 " bytes", m_socket.readable());
        on_error();
        return;
    }

    in_buffer *_buf = nullptr;
    {
#if (ON != KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_in_bufq_mutex);
#endif
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        assert(m_in_bufq.size() == m_message_handlerq.size());
#endif
        if (m_in_bufq.empty())
            return;
        _buf = &m_in_bufq.back();
    }
    assert(_buf and _buf->size());
    assert(_buf->valid() < _buf->size());

    std::error_code _ec;
    size_t _size = m_socket.read(*_buf, _ec);
    if (_ec) {
        if (((std::errc::operation_would_block == _ec or
              std::errc::resource_unavailable_try_again == _ec or
              std::errc::interrupted == _ec
              ) and m_socket.nonblock()
             ) or EINTR == errno
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
        on_error();
        return;
    }
    if (!_size) {
        on_close(_ec);
        return;
    } else {
        assert(m_callback_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
               ? _buf->size() >= m_callback_lowat : true);
        if (KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT != m_callback_lowat and
            _buf->valid() < m_callback_lowat)
            return;
        if (_buf->writeable())
            return;
        _buf = nullptr;

        // read done
        message_handler _handler = nullptr;
        in_buffer _back;
        {
#if (ON != KNGIN_SESSION_NO_MUTEX)
            local_lock _lock(m_in_bufq_mutex);
#endif
            _back = m_in_bufq.back();
            m_in_bufq.pop_back();
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
            _handler = m_message_handlerq.back();
            m_message_handlerq.pop_back();
#endif
        }
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
        if (m_message_handler) {
            log_excp_error(
                m_message_handler(std::ref(*this), _back, _back.valid()),
                "session::m_message_handler() error"
            );
        }
#else
        if (_handler) {
            log_excp_error(
                _handler(std::ref(*this), _back, _back.valid()),
                "session::m_message_handler() error"
            );
        }
#endif
    }
}

void
session::on_oob ()
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closed)
        return;
    if_not (pollpri())
        return;
    assert(_loop->in_loop_thread());

    //recv
    char _data;
    in_buffer _buf(&_data, 1);
    std::error_code _ec;
    size_t _size = m_socket.recv(_buf, MSG_OOB, _ec);
    if (_ec) {
        log_error("socket::recv(MSG_OOB) error, %s",
                  system_error_str(_ec).c_str());
        on_error();
        return;
    }
    if (!_size) {
        on_close(_ec);
        return;
    } else {
        if (m_oob_handler) {
            log_excp_error(
                m_oob_handler(std::ref(*this), _data),
                "session::m_oob_handler() error"
            );
        } else {
            log_warning("unhandled oob data from %s", m_socket.name().c_str());
        }
    }
}

void
session::on_error ()
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closed)
        return;
    assert(_loop->in_loop_thread());

    auto _ec = m_socket.read_error();
    if (_ec) {
        if (((std::errc::operation_would_block == _ec or
              std::errc::resource_unavailable_try_again == _ec or
              std::errc::interrupted == _ec
              ) and m_socket.nonblock()
             ) or EINTR == errno
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
        on_close(_ec);
        return;
    } else {
        tcp_info _info = sockopts::tcp_info(m_socket);
        if (_info.tcpi_state != TCP_ESTABLISHED)
            on_close(_ec);
        //log_debug("session::on_error(), no any error was readed");
        return;
    }
}

void
session::on_close ()
{
    if (m_closed)
        return;

    auto _self = self();
    m_socket.close();
    m_closed = true;
#if (ON != KNGIN_SESSION_TEMP_CALLBACK)
    clear_queues();
#endif
}

void
session::on_close (std::error_code _ec)
{
    if (m_closed)
        return;
    auto _loop = m_loop.lock();
    assert(_loop ? _loop->in_loop_thread() : true);

    auto _self = self();
    if (_loop and _loop->looping() and registed())
        _loop->remove_event(*this);
    m_socket.close();
    m_closed = true;
#if (ON != KNGIN_SESSION_TEMP_CALLBACK)
    clear_queues();
#endif
    if (m_close_handler) {
        log_excp_error(
            m_close_handler(std::cref(*this), _ec),
            "listener::m_close_handler() error"
        );
    }
}

void
session::clear_queues ()
{
    assert(m_closed);
    {
        local_lock _lock(m_out_bufq_mutex);
        m_out_bufq.clear();
    }
    {
        local_lock _lock(m_in_bufq_mutex);
        m_in_bufq.clear();
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

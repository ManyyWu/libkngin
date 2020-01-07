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
      m_in_ctxq(),
      m_out_ctxq(),
#endif
#if (ON == KNGIN_SESSION_ET_MODE)
      m_recv_complete(true),
      m_send_complete(true),
#endif
      m_oob_handler(nullptr),
      m_close_handler(nullptr),
#if (ON != KNGIN_SESSION_NO_MUTEX)
      m_out_bufq_mutex(),
      m_in_bufq_mutex(),
#endif
      m_key(m_peer_addr.key()),
      m_established(false)
{
    arg_check(m_socket.valid());

    // set socket options
    sockopts::set_ooblinline(m_socket, false);

    // set socket flags
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);

    // set event flags
    enable_read();
#if (OFF == KNGIN_SESSION_ET_MODE)
    disable_write();
    disable_oob();
#else
    enable_write();
    enable_oob();
#endif
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
    if (!_loop or m_closed or m_socket.wr_closed())
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
        m_out_ctxq.push_front({_buf, std::move(_handler)});
#else
        m_out_ctxq.push_front({_buf});
#endif
#if (OFF == KNGIN_SESSION_ET_MODE)
        if (m_out_ctxq.size() <= 1) {
            enable_write();
            _loop->update_event(*this);
        }
#endif
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
    if (!_loop or m_closed or m_socket.rd_closed())
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
        m_in_ctxq.push_front({_buf, std::move(_handler), _lowat});
#endif
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

    out_context *_out_ctx = nullptr;
    {
#if (ON != KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_out_bufq_mutex);
#endif
        if (m_out_ctxq.empty())
            return;
        _out_ctx = &m_out_ctxq.back();
    }
    assert(_out_ctx and _out_ctx->buffer.buffer().size());
    msg_buffer   &_buf = _out_ctx->buffer;

    std::error_code _ec;
    size_t _size = m_socket.write(_buf.buffer(), _ec);
    if (_ec) {
        if ((std::errc::operation_would_block == _ec or
             std::errc::resource_unavailable_try_again == _ec or
             std::errc::interrupted == _ec
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
        if (_buf.buffer().size())
            return;

        // write done
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
    sent_handler _handler = std::move(_out_ctx->handler);
#else
    sent_handler &_handler = m_sent_handler;
#endif
        shield_var(_buf);
        shield_var(_out_ctx);
        {
#if (ON != KNGIN_SESSION_NO_MUTEX)
            local_lock _lock(m_out_bufq_mutex);
#endif
            m_out_ctxq.pop_back();
#if (OFF == KNGIN_SESSION_ET_MODE)
            if (m_out_ctxq.empty()) {
                disable_write();
                _loop->update_event(*this);
            }
#endif
        }
        if (_handler) {
            log_excp_error(
                _handler(std::ref(*this)),
                "session::m_sent_handler() error"
            );
        }
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
    if (m_in_ctxq.empty()) {
        //log_debug("%" PRIu64 " bytes", m_socket.readable());
        on_error();
        return;
    }

    in_context *_in_ctx = nullptr;
    {
#if (ON != KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_in_bufq_mutex);
#endif
        if (m_in_ctxq.empty())
            return;
        _in_ctx = &m_in_ctxq.back();
    }
    assert(_in_ctx and _in_ctx->buffer.size() > _in_ctx->buffer.valid());
    in_buffer &_buf = _in_ctx->buffer;
    size_t    _lowat = _in_ctx->lowat;

    std::error_code _ec;
    size_t _size = m_socket.read(_buf, _ec);
    if (_ec) {
        if ((std::errc::operation_would_block == _ec or
             std::errc::resource_unavailable_try_again == _ec or
             std::errc::interrupted == _ec
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
        assert(_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
               ? _buf.size() >= _lowat : true);
        if (KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT != _lowat and
            _buf.valid() < _lowat)
            return;
        if (_buf.writeable())
            return;

        // read done
        shield_var(_buf);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        message_handler _handler = std::move(_in_ctx->handler);
#else
        message_handler &_handler = m_message_handler;
#endif
        in_context _in_ctx;
        {
#if (ON != KNGIN_SESSION_NO_MUTEX)
            local_lock _lock(m_in_bufq_mutex);
#endif
            _in_ctx = m_in_ctxq.back();
            m_in_ctxq.pop_back();
        }
        if (_handler) {
            log_excp_error(
                _handler(std::ref(*this), _in_ctx.buffer, _in_ctx.buffer.valid()),
                "session::m_message_handler() error"
            );
        }
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
        if (TCP_CLOSE_WAIT == _info.tcpi_state) { // peer are closed or wr_closed
            m_socket.rd_shutdown();
            on_close(_ec);
            return;
        }
        if (TCP_ESTABLISHED != _info.tcpi_state)
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

    if (_ec and !m_socket.wr_closed())
    {
        // send all meg_buffer
    }

    if (_loop->looping() and registed())
        _loop->remove_event(*this);
    m_socket.close();
    m_closed = true;
    clear_queues();
    if (m_established and m_close_handler) {
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
#if (OFF == KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_out_bufq_mutex);
#endif
        m_out_ctxq.clear();
    }
    {
#if (OFF == KNGIN_SESSION_NO_MUTEX)
        local_lock _lock(m_in_bufq_mutex);
#endif
        m_in_ctxq.clear();
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

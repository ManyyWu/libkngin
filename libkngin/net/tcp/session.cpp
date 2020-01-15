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

#if (ON == KNGIN_SESSION_NO_MUTEX)
#define SESSION_LOCAL_LOCK(m) static_cast<void>(0)
#else
#define SESSION_LOCAL_LOCK(m) local_lock _lock((m))
#endif

// for ET mode
#if (ON == KNGIN_SESSION_ET_MODE)
#define ET_MODE_ON_READ()              on_read();
#define ET_MODE_ON_WRITE()             on_write();
#define ET_MODE_ON_OOB()               on_oob();
#else
#define ET_MODE_ON_READ()              static_cast<void>(0)
#define ET_MODE_ON_WRITE()             static_cast<void>(0)
#define ET_MODE_ON_OOB()               static_cast<void>(0)
#endif

session::session (event_loop &_loop, k::socket &&_socket,
                  const address &_local_addr, const address &_peer_addr)
    try
    : epoller_event(_socket.fd(), epoller_event::EVENT_TYPE_FILE),
      m_loop(_loop.weak_self()),
      m_socket(std::move(_socket)), 
      m_closed(false),
      m_closing(false),
      m_local_addr(_local_addr),
      m_peer_addr(_peer_addr),
      m_name(m_socket.name()),
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
      m_message_handler(nullptr),
      m_sent_handler(nullptr),
#endif
      m_in_ctxq(),
      m_out_ctxq(),
      m_next_out_ctx(nullptr),
      m_next_in_ctx(nullptr),
#if (ON == KNGIN_SESSION_ET_MODE)
      m_recv_complete(true),
      m_send_complete(true),
#endif
      m_oob_handler(nullptr),
      m_error_handler(nullptr),
#if (ON != KNGIN_SESSION_NO_MUTEX)
      m_out_bufq_mutex(),
      m_in_bufq_mutex(),
#endif
      m_last_error(),
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
#if (OFF == KNGIN_SESSION_ET_MODE)
    disable_write();
    disable_oob();
#else
    enable_write();
    enable_oob();
    enable_et();
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
    assert(!m_socket.wr_closed());
    assert(_buf.buffer().begin() and _buf.buffer().size());

    {
        SESSION_LOCAL_LOCK(m_out_bufq_mutex);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        m_out_ctxq.push_front({_buf, std::move(_handler)});
#else
        m_out_ctxq.push_front({_buf});
#endif
        if (!m_next_out_ctx)
            m_next_out_ctx = &m_out_ctxq.back();
#if (OFF == KNGIN_SESSION_ET_MODE)
        if (m_out_ctxq.size() <= 1) {
            enable_write();
            _loop->update_event(*this);
        }
#endif
    }

    if (_loop->in_loop_thread()) {
        on_write();
    } else {
        _loop->run_in_loop([this] () {
            on_write();
        });
    }
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
    assert(!m_socket.rd_closed());
    assert(_buf.begin() and _buf.size());
    assert(_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
           ? _buf.size() >= _lowat : true);

    {
        SESSION_LOCAL_LOCK(m_in_bufq_mutex);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        m_in_ctxq.push_front({_buf, std::move(_handler), _lowat});
#else
        m_in_ctxq.push_front({_buf, _lowat});
#endif
        if (!m_next_in_ctx)
            m_next_in_ctx = &m_in_ctxq.back();
    }

    if (_loop->in_loop_thread()) {
        on_read();
    } else {
        _loop->run_in_loop([this] () {
            on_read();
        });
    }
    return true;
}

void
session::close (bool _blocking /* = false */)
{
    if (m_closed)
        return;
    m_closing = true;
    auto _loop = m_loop.lock();

    if (registed() and _loop and  _loop->looping()) {
        if (_loop->in_loop_thread()) {
            on_close();
        } else {
            if (_blocking) {
                auto _barrier_ptr = std::make_shared<barrier>(2);
                _loop->run_in_loop([this, _barrier_ptr] () {
                    on_close();
                    if (_barrier_ptr->wait())
                        _barrier_ptr->destroy();
                });
                if (_barrier_ptr->wait())
                    _barrier_ptr->destroy();
            } else {
                _loop->run_in_loop([this] () {
                    on_close();
                });
            }
        }
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
    if (!_loop or m_closing or m_closed or m_last_error or m_socket.wr_closed())
        return;
    auto _self = self();
    if_not (pollout())
        return;
    assert(_loop->in_loop_thread());
    if (!m_next_out_ctx)
        return;

    auto &_out_ctx = *m_next_out_ctx;
    auto &_buf = _out_ctx.buffer;
    assert(_buf.buffer().size());

    std::error_code _ec;
    size_t _size = m_socket.write(_buf.buffer(), _ec);
    if (_ec) {
        if (std::errc::operation_would_block == _ec or
            std::errc::resource_unavailable_try_again == _ec or
            std::errc::interrupted == _ec
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
        m_last_error = _ec;
        on_error();
        return;
    }
    if (!_size) {
        on_error();
        return;
    } else {
        if (_buf.buffer().size()) {
            ET_MODE_ON_WRITE();
            return;
        }

        // write done
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
    auto _handler = std::move(_out_ctx.handler);
#else
    auto &_handler = m_sent_handler;
#endif
        shield_var(_buf);
        shield_var(_out_ctx);
        {
            SESSION_LOCAL_LOCK(m_out_bufq_mutex);
            m_out_ctxq.pop_back();
            m_next_out_ctx = m_out_ctxq.empty() ? nullptr : &m_out_ctxq.back();
#if (OFF == KNGIN_SESSION_ET_MODE)
            if (!m_next_out_ctx) {
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
    ET_MODE_ON_WRITE();
}

void
session::on_read ()
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closing or m_closed or m_last_error or m_socket.rd_closed())
        return;
    auto _self = self();
    if_not (pollin())
        return;
    assert(_loop->in_loop_thread());
    //log_debug("session %s readable %" PRIu64 " bytes", name().c_str(), m_socket.readable());
    if (!m_next_in_ctx) {
        on_error();
        return;
    }

    auto &_in_ctx = *m_next_in_ctx;
    auto &_buf = _in_ctx.buffer;
    auto &_lowat = _in_ctx.lowat;
    assert(_buf.size() > _buf.valid());

    std::error_code _ec;
    size_t _size = m_socket.read(_buf, _ec);
    if (_ec) {
        if (std::errc::operation_would_block == _ec or
            std::errc::resource_unavailable_try_again == _ec or
            std::errc::interrupted == _ec
            )
            return;
        log_error("socket::write() error, %s",
                  system_error_str(_ec).c_str());
        m_last_error = _ec;
        on_error();
        return;
    }
    if (!_size) {
        on_error();
        return;
    } else {
        assert(_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
               ? _buf.size() >= _lowat : true);
        bool _writeable = false;
        if (KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT == _lowat) {
            if (_buf.writeable()) {
                ET_MODE_ON_READ();
                return;
            }
        } else {
            if (_buf.valid() < _lowat) {
                ET_MODE_ON_READ();
                return;
            }
            if (_buf.writeable()) {
                _lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT;
                _writeable = true;
            }
        }

        // read done
        if (_writeable) {
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
            auto &_handler = _in_ctx.handler;
#else
            auto &_handler = m_message_handler;
#endif
            if (_handler) {
                log_excp_error(
                    _handler(std::ref(*this), _buf, _buf.valid()),
                    "session::m_message_handler() error"
                );
            }
            return;
        }
        shield_var(_buf);
        shield_var(_in_ctx);
        in_context _back;
        {
            SESSION_LOCAL_LOCK(m_in_bufq_mutex);
            _back = m_in_ctxq.back();
            m_in_ctxq.pop_back();
            m_next_in_ctx = m_in_ctxq.empty() ? nullptr : &m_in_ctxq.back();
        }
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        auto &_handler = _back.handler;
#else
        auto &_handler = m_message_handler;
#endif
        if (_handler) {
            log_excp_error(
                _handler(std::ref(*this), _back.buffer, _back.buffer.valid()),
                "session::m_message_handler() error"
            );
        }
    }
    ET_MODE_ON_READ();
}

void
session::on_oob ()
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closing or m_closed or m_last_error or m_socket.rd_closed())
        return;
    auto _self = self();
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
        m_last_error = _ec;
        on_error();
        return;
    }
    if (!_size) {
        on_error();
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
    ET_MODE_ON_OOB();
}

void
session::on_error ()
{
    auto _loop = m_loop.lock();
    if (!_loop or m_closing or m_closed)
        return;
    auto _self = self();
    assert(_loop->in_loop_thread());

    if (m_last_error) {
_error:
        if (m_error_handler) {
            log_excp_error(
                m_error_handler(std::ref(*this), m_last_error),
                "session::m_error_handler() error"
            );
        }
    } else {
        static char _arr[1];
        in_buffer _buf(_arr, 1);
        std::error_code _ec;
        size_t _size = m_socket.recv(_buf, MSG_PEEK, _ec);
        if (_ec) {
            if (std::errc::operation_would_block == _ec or
                std::errc::resource_unavailable_try_again == _ec or
                std::errc::interrupted == _ec
                )
                return;
            log_error("socket::recv(MSG_PEEK) error, %s",
                      system_error_str(_ec).c_str());
            m_last_error = _ec;
            goto _error;
        }
        if (!_size)
            goto _error;
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
    auto _loop = m_loop.lock();
    if (registed() and _loop and  _loop->looping()) {
        assert(_loop->in_loop_thread());
        _loop->remove_event(*this);
    }
    m_socket.close();
    m_closed = true;
    clear_queues();
}

void
session::clear_queues ()
{
    assert(m_closed);
    {
        SESSION_LOCAL_LOCK(m_out_bufq_mutex);
        m_out_ctxq.clear();
    }
    {
        SESSION_LOCAL_LOCK(m_out_bufq_mutex);
        m_in_ctxq.clear();
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

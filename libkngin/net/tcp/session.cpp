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
      m_loop(_loop.pimpl()),
      m_socket(std::move(_socket)), 
      m_closed(true),
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
      m_out_bufq_mutex(),
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
      m_in_buf(),
#else
      m_in_bufq(),
#endif
      m_callback_lowat(KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT),
      m_key(m_peer_addr.key())
{
    m_socket.set_closeexec(true);
    m_socket.set_nonblock(true);
    sockopts::set_ooblinline(m_socket, false);
    enable_read();
    disable_write();
    disable_oob();
} catch (...) {
    log_fatal("session::session() error");
    throw;
}

session::~session () KNGIN_NOEXCP
{
    if (!m_closed || registed()) {
        log_warning("the TCP session must be closed"
                    " before object disconstructing");
        ignore_excp(this->close(true));
    }
    // FIXME; wait for m_closed to be true ( this->close(true); )
}

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)

bool
session::send (msg_buffer _buf)
{

    arg_check(_buf.buffer().begin() && _buf.buffer().size());
    if (m_closed)
        return false;

    {
        //local_lock _lock(m_out_bufq_mutex);
        m_out_bufq.push(_buf);
        if (m_out_bufq.size() <= 1) {
            enable_write();
            m_loop->update_event(self());
        }
    }

    if (m_loop->in_loop_thread())
        on_write();
    else
        m_loop->run_in_loop([this] () {
            on_write();
        });
    return true;
}

#else

bool
session::send (msg_buffer _buf, sent_handler &&_handler)
{
    arg_check(_buf.buffer().begin() && _buf.buffer().size());
    if (m_closed)
        return false;

    {
        //local_lock _lock(m_out_bufq_mutex);
        m_sent_handlerq.push(_handler);
        m_out_bufq.push(_buf);
        if (m_out_bufq.size() <= 1) {
            enable_write();
            m_loop->update_event(self());
        }
    }

    if (m_loop->in_loop_thread())
        on_write();
    else
        m_loop->run_in_loop([this] () {
            on_write();
        });
    return true;
}

#endif
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)

bool
session::recv (in_buffer _buf, size_t _lowat /* = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT */)
{
    arg_check(_buf.begin() && _buf.size());
    assert(!m_in_buf.begin() && !m_in_buf.size());
    assert(_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT ? _buf.size() >= _lowat : true);
    if (m_closed)
        return false;

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

#else

bool
session::recv (in_buffer _buf, message_handler &&_handler,
               size_t _lowat /* = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT */)
{
    arg_check(_buf.begin() && _buf.size());
    assert(_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT ? _buf.size() >= _lowat : true);
    if (m_closed)
        return false;

    {
        //local_lock _lock(m_in_bufq_mutex);
        m_message_handlerq.push(_handler);
        m_in_bufq.push(_buf);
        m_callback_lowat = _lowat;
    }

    if (m_loop->in_loop_thread())
        on_read();
    else
        m_loop->run_in_loop([this] () {
            on_read();
        });
    return true;
}

#endif

void
session::close (bool _blocking /* = false */)
{
    if (m_closed)
        return;
    if (!m_loop->looping()) {
        on_close(std::error_code());
        return;
    }
    if (registed()) {
        m_loop->remove_event(self());
        // XXX: Before calling shared_from_this,
        //      there should be at least one std::shared_ptr<T> p that owns *this.
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
}

void
session::rd_shutdown ()
{
    assert(!m_closed);
    assert(!m_socket.rd_closed());
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
    assert(!m_closed);
    assert(!m_socket.wr_closed());
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
    m_loop->check_thread();
    if (m_closed)
        return;
    if_not (pollout())
        return;

    msg_buffer *_buf = nullptr;
    {
        //local_lock _lock(m_out_bufq_mutex);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        assert(m_out_bufq.size() == m_sent_handlerq.size());
#endif
        if (m_out_bufq.empty())
            return;
        _buf = &m_out_bufq.front();
    }
    assert(_buf && _buf->buffer().size());

    std::error_code _ec;
    size_t _size = m_socket.write(_buf->buffer(), _ec);
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
        if (_buf->buffer().size())
            return;
        _buf = nullptr;

        // write done
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        sent_handler _handler = nullptr;
#endif
        {
            //local_lock _lock(m_out_bufq_mutex);
            m_out_bufq.pop();
            if (m_out_bufq.empty()) {
                disable_write();
                m_loop->update_event(self());
            }
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
            _handler = m_sent_handlerq.front();
            m_sent_handlerq.pop();
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
    m_loop->check_thread();
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    if (!m_in_buf.begin()) {
#else
    if (m_in_bufq.empty()) {
#endif
        //log_debug("%" PRIu64 " bytes", m_socket.readable());
        on_error();
        return;
    }
    if (m_closed)
        return;
    if_not (pollin())
        return;

    in_buffer *_buf = nullptr;
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    _buf = &m_in_buf;
#else
    {
        //local_lock _lock(m_in_bufq_mutex);
        assert(m_in_bufq.size() == m_message_handlerq.size());
        if (m_in_bufq.empty())
            return;
        _buf = &m_in_bufq.front();
    }
#endif
    assert(_buf && _buf->size());
    assert(_buf->valid() < _buf->size());

    std::error_code _ec;
    size_t _size = m_socket.read(*_buf, _ec);
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
        assert(m_callback_lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT ? _buf->size() >= m_callback_lowat : true);
        if (KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT != m_callback_lowat &&
            _buf->valid() < m_callback_lowat)
            return;
        if (_buf->writeable())
            return;
        _buf = nullptr;

        // read done
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
        in_buffer _back = m_in_buf;
        m_in_buf = in_buffer();
#else
        message_handler _handler = nullptr;
        in_buffer _back;
        {
            //local_lock _lock(m_in_bufq_mutex);
            _back = m_in_bufq.front();
            m_in_bufq.pop();
            _handler = m_message_handlerq.front();
            m_message_handlerq.pop();
        }
#endif
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
    if (m_closed)
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
    if (m_closed)
        return;
    m_loop->check_thread();

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
        on_close(_ec);
#warning "error_code"
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
session::on_close (std::error_code _ec)
{
    if (m_closed)
        return;
    m_loop->check_thread();

    auto _self = self(); // extend the life cycle untile closed
    if (m_loop->looping() && registed())
        m_loop->remove_event(self());
    m_socket.close();
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    while (m_out_bufq.size());
        m_out_bufq.pop();
    m_in_buf = in_buffer();
#else
    while (m_out_bufq.size())
        m_out_bufq.pop();
    while (m_in_bufq.size())
        m_in_bufq.pop();
#endif
    m_closed = true;
    if (m_close_handler) {
        log_excp_error(
            m_close_handler(std::cref(*this), _ec),
            "listener::m_close_handler() error"
        );
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

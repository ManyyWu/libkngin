#include "core/base/common.h"
#include "net/tcp/session.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/base/session.cpp"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

#if (ON == KNGIN_SESSION_NO_MUTEX)
#define SESSION_LOCAL_LOCK(m) static_cast<void>(0)
#else
#define SESSION_LOCAL_LOCK(m) local_lock lock((m))
#endif

// for ET mode
#if (ON == KNGIN_SESSION_ET_MODE)
#define ET_MODE_ON_READ()  on_read();
#define ET_MODE_ON_WRITE() on_write();
#define ET_MODE_ON_OOB()   on_oob();
#else
#define ET_MODE_ON_READ()  static_cast<void>(0)
#define ET_MODE_ON_WRITE() static_cast<void>(0)
#define ET_MODE_ON_OOB()   static_cast<void>(0)
#endif

session::session (event_loop &loop, k::socket &&socket,
                  const address &local_addr, const address &peer_addr)
    try
    : epoller_event(socket.fd(), epoller_event::EVENT_TYPE_FILE),
      m_loop(&loop),
      m_socket(std::move(socket)),
      m_closed(false),
      m_closing(false),
      m_local_addr(local_addr),
      m_peer_addr(peer_addr),
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

    // set file flags
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

session::~session () noexcept
{
    if (!m_closed or registed()) {
        log_warning("the TCP session must be closed before object disconstructing");
        ignore_excp(this->close(true));
    }
    // FIXME; wait for m_closed to be true ( this->close(true); )
}

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
bool
session::send (msg_buffer buf)
#else
bool
session::send (msg_buffer buf, sent_handler &&handler)
#endif
{
    if (m_closed or m_socket.wr_closed())
        return false;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(m_loop->in_loop_thread());
#endif
    assert(!m_socket.wr_closed());
    assert(buf.buffer().begin() and buf.buffer().size());

    {
        SESSION_LOCAL_LOCK(m_out_bufq_mutex);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        m_out_ctxq.push_front({buf, std::move(handler)});
#else
        m_out_ctxq.push_front({buf});
#endif
        if (!m_next_out_ctx)
            m_next_out_ctx = &m_out_ctxq.back();
#if (OFF == KNGIN_SESSION_ET_MODE)
        if (m_out_ctxq.size() <= 1) {
            enable_write();
            m_loop->update_event(*this);
        }
#endif
    }

    if (m_loop->in_loop_thread()) {
        on_write();
    } else {
        auto self_weak_ptr = weak_ptr();
        m_loop->run_in_loop([self_weak_ptr] () {
            auto s = self_weak_ptr.lock();
            if (s)
                s->on_write();
        });
    }
    return true;
}

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
bool
session::recv (in_buffer buf, size_t lowat /* = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT */)
#else
bool
session::recv (in_buffer buf, message_handler &&handler,
               size_t lowat /* = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT */)
#endif
{
    if (m_closed or m_socket.rd_closed())
        return false;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(m_loop->in_loop_thread());
#endif
    assert(!m_socket.rd_closed());
    assert(buf.begin() and buf.size());
    assert(lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
           ? buf.size() >= lowat : true);

    {
        SESSION_LOCAL_LOCK(m_in_bufq_mutex);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        m_in_ctxq.push_front({buf, std::move(handler), lowat});
#else
        m_in_ctxq.push_front({buf, lowat});
#endif
        if (!m_next_in_ctx)
            m_next_in_ctx = &m_in_ctxq.back();
    }

    if (m_loop->in_loop_thread()) {
        on_read();
    } else {
        auto self_weak_ptr = weak_ptr();
        m_loop->run_in_loop([self_weak_ptr] () {
            auto s = self_weak_ptr.lock();
            if (s)
                s->on_read();
        });
    }
    return true;
}

void
session::close (bool sync /* = false */)
{
    if (m_closed)
        return;
    m_closing = true;

    if (registed() and m_loop->looping()) {
        if (m_loop->in_loop_thread()) {
            on_close();
        } else {
            auto self_weak_ptr = weak_ptr();
            if (sync) {
                auto barrier_ptr = std::make_shared<barrier>(2);
                m_loop->run_in_loop([self_weak_ptr, barrier_ptr] () {
                    auto s = self_weak_ptr.lock();
                    if (s)
                        s->on_close();
                    if (barrier_ptr->wait())
                        barrier_ptr->destroy();
                });
                if (barrier_ptr->wait())
                    barrier_ptr->destroy();
            } else {
                m_loop->run_in_loop([self_weak_ptr] () {
                    auto s = self_weak_ptr.lock();
                    if (s)
                        s->on_close();
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
    if (!m_loop->looping())
        return;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(m_loop->in_loop_thread());
#endif
    assert(!m_closed);
    assert(!m_socket.rd_closed());
    if (m_socket.wr_closed())
        this->close();
    if (m_loop->in_loop_thread()) {
        m_socket.rd_shutdown();
    } else {
        auto self_weak_ptr = weak_ptr();
        m_loop->run_in_loop([self_weak_ptr] () {
            auto s = self_weak_ptr.lock();
            if (s)
                s->m_socket.rd_shutdown();
        });
    }
}

void
session::wr_shutdown ()
{
    if (!m_loop->looping())
        return;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(m_loop->in_loop_thread());
#endif
    assert(!m_closed);
    assert(!m_socket.wr_closed());
    if (m_socket.rd_closed())
        this->close();
    if (m_loop->in_loop_thread()) {
        m_socket.wr_shutdown();
    } else {
        auto self_weak_ptr = weak_ptr();
        m_loop->run_in_loop([self_weak_ptr] () {
            auto s = self_weak_ptr.lock();
            if (s)
                s->m_socket.wr_shutdown();
        });
    }
}

void
session::on_events (event_loop &loop, uint32_t flags)
{
    assert(m_loop == &loop);
    try {
        if ((EPOLLHUP | EPOLLERR) & flags) {
            this->on_error();
            return;
        }
        if (EPOLLIN & flags)
            this->on_read();
        if (EPOLLOUT & flags)
            this->on_write();
        if (EPOLLPRI & flags)
            this->on_oob();
    } catch (std::exception &e) {
        log_fatal("caught an exception in session::on_event(), %s", e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in session::on_event()");
        throw;
    }
}

void
session::on_write ()
{
    if (m_closing or m_closed or m_last_error or m_socket.wr_closed())
        return;
    if (!m_next_out_ctx)
        return;
    auto s = self();

    auto &out_ctx = *m_next_out_ctx;
    auto &buf = out_ctx.buffer;
    assert(buf.buffer().size());

    error_code ec;
    auto size = m_socket.write(buf.buffer(), ec);
    if (ec) {
        if (EWOULDBLOCK == ec or
            EAGAIN == ec or
            EINTR == ec
            )
            return;
        log_error("socket::write() error, %s", system_error_str(ec).c_str());
        m_last_error = ec;
        on_error();
        return;
    }
    if (!size) {
        on_error();
        return;
    } else {
        if (buf.buffer().size()) {
            ET_MODE_ON_WRITE();
            return;
        }

        // write done
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
    auto handler = std::move(out_ctx.handler);
#else
    auto &handler = m_sent_handler;
#endif
        shield_var(buf);
        shield_var(out_ctx);
        {
            SESSION_LOCAL_LOCK(m_out_bufq_mutex);
            m_out_ctxq.pop_back();
            m_next_out_ctx = m_out_ctxq.empty() ? nullptr : &m_out_ctxq.back();
#if (OFF == KNGIN_SESSION_ET_MODE)
            if (!m_next_out_ctx) {
                disable_write();
                loop->update_event(*this);
            }
#endif
        }
        if (handler) {
            log_excp_error(
                handler(std::ref(*this)),
                "session::m_sent_handler() error"
            );
        }
    }
    ET_MODE_ON_WRITE();
}

void
session::on_read ()
{
    if (m_closing or m_closed or m_last_error or m_socket.rd_closed())
        return;
    //log_debug("session %s readable %" PRIu64 " bytes", name().c_str(), m_socket.readable());
    if (!m_next_in_ctx) {
        on_error();
        return;
    }
    auto s = self();

    auto &in_ctx = *m_next_in_ctx;
    auto &buf = in_ctx.buffer;
    auto &lowat = in_ctx.lowat;
    assert(buf.size() > buf.valid());

    error_code ec;
    auto size = m_socket.read(buf, ec);
    if (ec) {
        if (EWOULDBLOCK == ec or
            EAGAIN == ec or
            EINTR == ec
            )
            return;
        log_error("socket::write() error, %s", system_error_str(ec).c_str());
        m_last_error = ec;
        on_error();
        return;
    }
    if (!size) {
        on_error();
        return;
    } else {
        assert(lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
               ? buf.size() >= lowat : true);
        bool writeable = false;
        if (KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT == lowat) {
            if (buf.writeable()) {
                ET_MODE_ON_READ();
                return;
            }
        } else {
            if (buf.valid() < lowat) {
                ET_MODE_ON_READ();
                return;
            }
            if (buf.writeable()) {
                lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT;
                writeable = true;
            }
        }

        // read done
        if (writeable) {
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
            auto &handler = in_ctx.handler;
#else
            auto &handler = m_message_handler;
#endif
            if (handler) {
                log_excp_error(
                    handler(std::ref(*this), buf, buf.valid()),
                    "session::m_message_handler() error"
                );
            }
            return;
        }
        shield_var(buf);
        shield_var(in_ctx);
        in_context back;
        {
            SESSION_LOCAL_LOCK(m_in_bufq_mutex);
            back = m_in_ctxq.back();
            m_in_ctxq.pop_back();
            m_next_in_ctx = m_in_ctxq.empty() ? nullptr : &m_in_ctxq.back();
        }
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        auto &handler = back.handler;
#else
        auto &handler = m_message_handler;
#endif
        if (handler) {
            log_excp_error(
                handler(std::ref(*this), back.buffer, back.buffer.valid()),
                "session::m_message_handler() error"
            );
        }
    }
    ET_MODE_ON_READ();
}

void
session::on_oob ()
{
    if (m_closing or m_closed or m_last_error or m_socket.rd_closed())
        return;
    auto s = self();

    //recv
    char data;
    in_buffer buf(&data, 1);
    error_code ec;
    auto size = m_socket.recv(buf, MSG_OOB, ec);
    if (ec) {
        log_error("socket::recv(MSG_OOB) error, %s",
                  system_error_str(ec).c_str());
        m_last_error = ec;
        on_error();
        return;
    }
    if (!size) {
        on_error();
        return;
    } else {
        if (m_oob_handler) {
            log_excp_error(
                m_oob_handler(std::ref(*this), data),
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
    if (m_closing or m_closed)
        return;
    auto s = self();

    if (m_last_error) {
error:
        if (m_error_handler) {
            log_excp_error(
                m_error_handler(std::ref(*this), m_last_error),
                "session::m_error_handler() error"
            );
        }
    } else {
        static char arr[1];
        in_buffer buf(arr, 1);
        error_code ec;
        auto size = m_socket.recv(buf, MSG_PEEK, ec);
        if (ec) {
            if (EWOULDBLOCK == ec or
                EAGAIN == ec or
                EINTR == ec
                )
                return;
            log_error("socket::recv(MSG_PEEK) error, %s", system_error_str(ec).c_str());
            m_last_error = ec;
            goto error;
        }
        if (!size)
            goto error;
        //log_debug("session::on_error(), no any error was readed");
        return;
    }
}

void
session::on_close ()
{
    if (m_closed)
        return;

    if (registed() and m_loop->looping())
        m_loop->remove_event(*this);
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

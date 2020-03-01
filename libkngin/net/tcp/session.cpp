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
      loop_(&loop),
      socket_(std::move(socket)),
      closed_(false),
      closing_(false),
      local_addr_(local_addr),
      peer_addr_(peer_addr),
      name_(socket_.name()),
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
      message_handler_(nullptr),
      sent_handler_(nullptr),
#endif
      in_ctxq_(),
      out_ctxq_(),
      next_out_ctx_(nullptr),
      next_in_ctx_(nullptr),
#if (ON == KNGIN_SESSION_ET_MODE)
      recv_complete_(true),
      send_complete_(true),
#endif
      oob_handler_(nullptr),
      error_handler_(nullptr),
#if (ON != KNGIN_SESSION_NO_MUTEX)
      out_bufq_mutex_(),
      in_bufq_mutex_(),
#endif
      last_error_(),
      key_(peer_addr_.key())
{
    arg_check(socket_.valid());

    // set socket options
    sockopts::set_ooblinline(socket_, false);

    // set file flags
    socket_.set_closeexec(true);
    socket_.set_nonblock(true);

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
    if (!closed_ or registed()) {
        log_warning("the TCP session must be closed before object disconstructing");
        ignore_excp(this->close(true));
    }
    // FIXME; wait for closed_ to be true ( this->close(true); )
}

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
bool
session::send (msg_buffer buf)
#else
bool
session::send (msg_buffer buf, sent_handler &&handler)
#endif
{
    if (closed_ or socket_.wr_closed())
        return false;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(loop_->in_loop_thread());
#endif
    assert(!socket_.wr_closed());
    assert(buf.buffer().begin() and buf.buffer().size());

    {
        SESSION_LOCAL_LOCK(out_bufq_mutex_);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        out_ctxq_.push_front({buf, std::move(handler)});
#else
        out_ctxq_.push_front({buf});
#endif
        if (!next_out_ctx_)
            next_out_ctx_ = &out_ctxq_.back();
#if (OFF == KNGIN_SESSION_ET_MODE)
        if (out_ctxq_.size() <= 1) {
            enable_write();
            loop_->update_event(*this);
        }
#endif
    }

    if (loop_->in_loop_thread()) {
        on_write();
    } else {
        auto self_weak_ptr = weak_ptr();
        loop_->run_in_loop([self_weak_ptr] () {
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
    if (closed_ or socket_.rd_closed())
        return false;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(loop_->in_loop_thread());
#endif
    assert(!socket_.rd_closed());
    assert(buf.begin() and buf.size());
    assert(lowat != KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT
           ? buf.size() >= lowat : true);

    {
        SESSION_LOCAL_LOCK(in_bufq_mutex_);
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        in_ctxq_.push_front({buf, std::move(handler), lowat});
#else
        in_ctxq_.push_front({buf, lowat});
#endif
        if (!next_in_ctx_)
            next_in_ctx_ = &in_ctxq_.back();
    }

    if (loop_->in_loop_thread()) {
        on_read();
    } else {
        auto self_weak_ptr = weak_ptr();
        loop_->run_in_loop([self_weak_ptr] () {
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
    if (closed_)
        return;
    closing_ = true;

    if (registed() and loop_->looping()) {
        if (loop_->in_loop_thread()) {
            on_close();
        } else {
            auto self_weak_ptr = weak_ptr();
            if (sync) {
                auto barrier_ptr = std::make_shared<barrier>(2);
                loop_->run_in_loop([self_weak_ptr, barrier_ptr] () {
                    auto s = self_weak_ptr.lock();
                    if (s)
                        s->on_close();
                    if (barrier_ptr->wait())
                        barrier_ptr->destroy();
                });
                if (barrier_ptr->wait())
                    barrier_ptr->destroy();
            } else {
                loop_->run_in_loop([self_weak_ptr] () {
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
    if (!loop_->looping())
        return;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(loop_->in_loop_thread());
#endif
    assert(!closed_);
    assert(!socket_.rd_closed());
    if (socket_.wr_closed())
        this->close();
    if (loop_->in_loop_thread()) {
        socket_.rd_shutdown();
    } else {
        auto self_weak_ptr = weak_ptr();
        loop_->run_in_loop([self_weak_ptr] () {
            auto s = self_weak_ptr.lock();
            if (s)
                s->socket_.rd_shutdown();
        });
    }
}

void
session::wr_shutdown ()
{
    if (!loop_->looping())
        return;
#if (ON == KNGIN_SESSION_NO_MUTEX)
    assert(loop_->in_loop_thread());
#endif
    assert(!closed_);
    assert(!socket_.wr_closed());
    if (socket_.rd_closed())
        this->close();
    if (loop_->in_loop_thread()) {
        socket_.wr_shutdown();
    } else {
        auto self_weak_ptr = weak_ptr();
        loop_->run_in_loop([self_weak_ptr] () {
            auto s = self_weak_ptr.lock();
            if (s)
                s->socket_.wr_shutdown();
        });
    }
}

void
session::on_events (event_loop &loop, uint32_t flags)
{
    assert(loop_ == &loop);
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
    if (closing_ or closed_ or last_error_ or socket_.wr_closed())
        return;
    if (!next_out_ctx_)
        return;
    auto s = self();

    auto &out_ctx = *next_out_ctx_;
    auto &buf = out_ctx.buffer;
    assert(buf.buffer().size());

    error_code ec;
    auto size = socket_.write(buf.buffer(), ec);
    if (ec) {
        if (EWOULDBLOCK == ec or
            EAGAIN == ec or
            EINTR == ec
            )
            return;
        log_error("socket::write() error, %s", system_error_str(ec).c_str());
        last_error_ = ec;
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
    auto &handler = sent_handler_;
#endif
        shield_var(buf);
        shield_var(out_ctx);
        {
            SESSION_LOCAL_LOCK(out_bufq_mutex_);
            out_ctxq_.pop_back();
            next_out_ctx_ = out_ctxq_.empty() ? nullptr : &out_ctxq_.back();
#if (OFF == KNGIN_SESSION_ET_MODE)
            if (!next_out_ctx_) {
                disable_write();
                loop->update_event(*this);
            }
#endif
        }
        if (handler) {
            log_excp_error(
                handler(std::ref(*this)),
                "session::sent_handler_() error"
            );
        }
    }
    ET_MODE_ON_WRITE();
}

void
session::on_read ()
{
    if (closing_ or closed_ or last_error_ or socket_.rd_closed())
        return;
    //log_debug("session %s readable %" PRIu64 " bytes", name().c_str(), socket_.readable());
    if (!next_in_ctx_) {
        on_error();
        return;
    }
    auto s = self();

    auto &in_ctx = *next_in_ctx_;
    auto &buf = in_ctx.buffer;
    auto &lowat = in_ctx.lowat;
    assert(buf.size() > buf.valid());

    error_code ec;
    auto size = socket_.read(buf, ec);
    if (ec) {
        if (EWOULDBLOCK == ec or
            EAGAIN == ec or
            EINTR == ec
            )
            return;
        log_error("socket::write() error, %s", system_error_str(ec).c_str());
        last_error_ = ec;
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
            auto &handler = message_handler_;
#endif
            if (handler) {
                log_excp_error(
                    handler(std::ref(*this), buf, buf.valid()),
                    "session::message_handler_() error"
                );
            }
            return;
        }
        shield_var(buf);
        shield_var(in_ctx);
        in_context back;
        {
            SESSION_LOCAL_LOCK(in_bufq_mutex_);
            back = in_ctxq_.back();
            in_ctxq_.pop_back();
            next_in_ctx_ = in_ctxq_.empty() ? nullptr : &in_ctxq_.back();
        }
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        auto &handler = back.handler;
#else
        auto &handler = message_handler_;
#endif
        if (handler) {
            log_excp_error(
                handler(std::ref(*this), back.buffer, back.buffer.valid()),
                "session::message_handler_() error"
            );
        }
    }
    ET_MODE_ON_READ();
}

void
session::on_oob ()
{
    if (closing_ or closed_ or last_error_ or socket_.rd_closed())
        return;
    auto s = self();

    //recv
    char data;
    in_buffer buf(&data, 1);
    error_code ec;
    auto size = socket_.recv(buf, MSG_OOB, ec);
    if (ec) {
        log_error("socket::recv(MSG_OOB) error, %s",
                  system_error_str(ec).c_str());
        last_error_ = ec;
        on_error();
        return;
    }
    if (!size) {
        on_error();
        return;
    } else {
        if (oob_handler_) {
            log_excp_error(
                oob_handler_(std::ref(*this), data),
                "session::oob_handler_() error"
            );
        } else {
            log_warning("unhandled oob data from %s", socket_.name().c_str());
        }
    }
    ET_MODE_ON_OOB();
}

void
session::on_error ()
{
    if (closing_ or closed_)
        return;
    auto s = self();

    if (last_error_) {
error:
        if (error_handler_) {
            log_excp_error(
                error_handler_(std::ref(*this), last_error_),
                "session::error_handler_() error"
            );
        }
    } else {
        static char arr[1];
        in_buffer buf(arr, 1);
        error_code ec;
        auto size = socket_.recv(buf, MSG_PEEK, ec);
        if (ec) {
            if (EWOULDBLOCK == ec or
                EAGAIN == ec or
                EINTR == ec
                )
                return;
            log_error("socket::recv(MSG_PEEK) error, %s", system_error_str(ec).c_str());
            last_error_ = ec;
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
    if (closed_)
        return;

    if (registed() and loop_->looping())
        loop_->remove_event(*this);
    socket_.close();
    closed_ = true;
    clear_queues();
}

void
session::clear_queues ()
{
    assert(closed_);
    {
        SESSION_LOCAL_LOCK(out_bufq_mutex_);
        out_ctxq_.clear();
    }
    {
        SESSION_LOCAL_LOCK(out_bufq_mutex_);
        in_ctxq_.clear();
    }
}

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#include <sudo_plugin.h>
#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_TCP_SESSION)

#include "kngin/core/base/system_error.h"
#include "kngin/core/base/exception.h"
#include "detail/net/tcp/impl/posix_tcp_ostream.h"
#include "detail/net/tcp/impl/posix_session.h"

namespace k::detail::impl {

namespace tcp {

posix_session::ostream::ostream (posix_session &session, socket &sock,
                                 write_handler &&o_cb)
 : session_(session),
   write_handler_(std::move(o_cb)),
   oobq_(),
   buffers_(),
   rindex_(0),
   windex_(0),
   buffers_mutex_(),
   next_buffer_(nullptr),
   complete_(false) {
  buffers_.push_back(new uint8_t[KNGIN_OUT_BUFFER_SIZE]);
}

posix_session::ostream::~ostream () noexcept {
  TRY()
    oobq_.clear();
  IGNORE_EXCP("posix_session::ostream::~ostream")
}

void
posix_session::ostream::async_write (const out_buffer &buf, int flags) {
  assert(buf.size());
  {
    auto size = buf.size();
    mutex::scoped_lock lock(buffers_mutex_);
    bool enable_write = !next_buffer_;
    if (flags & socket::message_oob) {
      if (size > 1) {
        out_buffer temp(buf.begin(), size - 1);
        append_buffer(temp);
      }
      oobq_.push_back(buf[size - 1]);
    } else {
      append_buffer(buf);
    }
    if (enable_write and !session_.ev_.et()) {
      session_.ev_.enable_write();
      session_.loop_.update_event(session_.ev_);
    }
  }
  if (!complete_ and session_.ev_.et())
    session_.loop_.run_in_loop([this]() { on_write(); });
}

size_t
posix_session::ostream::async_write (const out_buffers &bufs) {
  size_t done = 0;
  if (bufs.size())
  {
    mutex::scoped_lock lock(buffers_mutex_);
    bool enable_write = !next_buffer_;
    try {
      for (auto &iter : bufs) {
        append_buffer(iter);
        ++done;
      }
    } catch (...) {
    }
    if (enable_write and !session_.ev_.et()) {
      session_.ev_.enable_write();
      session_.loop_.update_event(session_.ev_);
    }
  }
  if (!complete_ and session_.ev_.et())
    session_.loop_.run_in_loop([this]() { on_write(); });
  return done;
}

void
posix_session::ostream::clear () {
  {
    mutex::scoped_lock lock(buffers_mutex_);
    while (buffers_.size ()) {
      safe_release(buffers_.back());
      buffers_.pop_back();
    }
    oobq_.clear();
    next_buffer_ = nullptr;
    rindex_ = 0;
  }
}

void
posix_session::ostream::on_write () {
  do {
    if (session_.flags_ & (flag_shutdown | flag_closed | flag_closing))
      break;
    if (oobq_.size()) {
      write_oob();
      continue;
    }
    if (!next_buffer_ and !complete_) {
      if (!session_.ev_.et() and session_.ev_.out()) {
        session_.ev_.disable_write();
        session_.loop_.update_event(session_.ev_);
      }
      break;
    }
    complete_ = false;

    auto valid = (std::min<size_t>)(windex_ - rindex_, KNGIN_OUT_BUFFER_SIZE - rindex_);
    assert(valid > 0);

    error_code ec;
    out_buffer buf(buffers_.front() + rindex_, valid);
#if (0 and !defined(NDEBUG))
    log_debug("next: %s, ridx: %lld, widx: %lld",
              buf.dump().c_str(), rindex_.load(), windex_.load());
#endif /* !defined(NDEBUG) */
    auto size = session_.socket_.send(buf, 0, ec);
    if (ec) {
      if (KNGIN_EINTR == ec)
        continue;
      if (KNGIN_EAGAIN == ec) {
        if (session_.ev_.et()) {
          complete_ = true;
        } else {
          session_.ev_.disable_write();
          session_.loop_.update_event(session_.ev_);
        }
        break;
      }
      if (KNGIN_ECONNRESET == ec || KNGIN_EPIPE == ec)
        session_.flags_ |= flag_reset;
      session_.flags_ |= flag_error;
      session_.last_error_ = ec;
      on_complete(0, ec);
      break;
    }
    session_.flags_ &= ~flag_error;
    if (!size) {
      continue;
    } else {
      assert(size <= valid);
      if (size < valid)
        continue;

      // complete
      {
        mutex::scoped_lock lock(buffers_mutex_);
        rindex_ += size;
        assert(rindex_ <= KNGIN_OUT_BUFFER_SIZE);
        if (KNGIN_OUT_BUFFER_SIZE == rindex_ || rindex_ == windex_) {
          rindex_ = 0;
          windex_ -= size;
          if (buffers_.size() > 1)
            buffers_.erase(buffers_.begin());
          next_buffer_ = (windex_ - rindex_) ? buffers_.front() : nullptr;
        }
      }
      on_complete(size, ec);
    }
  } while (true);
}

void
posix_session::ostream::on_complete(size_t size, const error_code &ec) {
  TRY ()
    if (write_handler_)
      write_handler_(session_.owner_, size, ec);
  IGNORE_EXCP("posix_session::istream::message_callback")
}

void
posix_session::ostream::write_oob () {
  uint8_t data;
  bool empty = true;
  do {
    {
      mutex::scoped_lock lock(buffers_mutex_);
      if (!empty and oobq_.size())
        oobq_.erase(oobq_.begin());
      empty = oobq_.empty();
      if (empty)
        break;
      data = oobq_.front();
    }
    error_code ec;
    out_buffer buf(&data, 1);
    session_.socket_.send(buf, socket::message_oob, ec);
    if (ec) {
      if (KNGIN_EINTR == ec)
        continue;
      if (KNGIN_EAGAIN == ec) {
        complete_ = true;
        break;
      }
      if (KNGIN_ECONNRESET == ec || KNGIN_EPIPE == ec)
        session_.flags_ |= flag_reset;
      session_.flags_ |= flag_error;
      session_.last_error_ = ec;
      on_complete(0, ec);
      break;
    }
    session_.flags_ &= ~flag_error;
  } while (true);
}

void
posix_session::ostream::append_buffer (const k::out_buffer &buf) {
  static_assert((KNGIN_OUT_BUFFER_SIZE > 0), "invalid KNGIN_OUT_BUFFER_SIZE value");
  static const size_t buffer_size = KNGIN_OUT_BUFFER_SIZE;
  const size_t size = buf.size();
  const uint8_t *data = buf.begin();
  assert(size > 0);
  if (size) {
    while (windex_ + size > buffer_size * buffers_.size())
      buffers_.push_back(new uint8_t[buffer_size]);

    size_t which = (windex_ / buffer_size);
    size_t new_windex = windex_ + size;
    size_t begin = 0;
    do {
      size_t index = windex_ % buffer_size;
      size_t writeable = (std::min)(size - begin, buffer_size - index);
      ::memcpy(buffers_[which] + index, data, writeable);
      windex_ += writeable;
      if (windex_ >= new_windex)
        break;
      data += writeable;
      ++which;
      begin += writeable;
    } while (true);
    if (!next_buffer_)
      next_buffer_ = buffers_.front();
  }
#if (0 and !defined(NDEBUG))
    {
      for (auto &iter : buffers_)
        log_debug("%s", out_buffer(iter, KNGIN_OUT_BUFFER_SIZE).dump().c_str());
      log_debug("ridx: %lld, widx: %lld", rindex_.load(), windex_.load());
      log_debug("");
    }
#endif /* !defined(DEGBUG) */
}

} /* namespace tcp */

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_TCP_SESSION) */

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_TCP_SESSION)

#include "kngin/core/base/system_error.h"
#include "kngin/core/base/exception.h"
#include "detail/net/tcp/impl/posix_tcp_istream.h"
#include "detail/net/tcp/impl/posix_session.h"

namespace k::detail::impl {

namespace tcp {

posix_session::istream::istream (posix_session &session, socket &sock,
                                 message_handler &&i_cb, oob_handler &&oob_cb)
 : session_(session),
   message_handler_(std::move(i_cb)),
   oob_handler_(std::move(oob_cb)),
   ctxq_(),
   ctxq_mutex_(),
   next_ctx_(nullptr),
   complete_(false) {
}

posix_session::istream::~istream () noexcept {
  TRY()
    ctxq_.clear();
  IGNORE_EXCP("posix_session::istream::~istream")
}

void
posix_session::istream::async_read (in_buffer &buf) {
  assert(buf.writeable());
  {
    mutex::scoped_lock lock(ctxq_mutex_);
    ctxq_.push_front({buf, true});
    if (!next_ctx_)
      next_ctx_ = &ctxq_.back();
  }
  if (!complete_ and session_.ev_.et())
    session_.loop_.run_in_loop([this]() { on_read(); });
}

void
posix_session::istream::async_read_some (in_buffer &buf) {
  assert(buf.writeable());
  {
    mutex::scoped_lock lock(ctxq_mutex_);
    ctxq_.push_front({buf, false});
    if (!next_ctx_)
      next_ctx_ = &ctxq_.back();
  }
  if (!complete_ and session_.ev_.et())
    session_.loop_.run_in_loop([this]() { on_read(); });
}

void
posix_session::istream::clear () {
  {
    mutex::scoped_lock lock(ctxq_mutex_);
    ctxq_.clear();
    next_ctx_ = nullptr;
  }
}

void
posix_session::istream::on_read () {
  assert(!session_.is_eof());
  do {
    if (session_.flags_ & (flag_closed | flag_closing))
      break;
    if (!next_ctx_ and !complete_)
      break; // wait for new post
    complete_ = false;

    auto &ctx = *next_ctx_; // it can't be use after call message_callback
    auto &buf = ctx.buffer; // it can't be use after call message_callback
    auto need_complete = ctx.need_complete;
    assert(buf.writeable());

    error_code ec;
    auto size = session_.socket_.recv(buf, 0, ec);
    if (ec) {
      if (KNGIN_EINTR == ec)
        continue;
      if (KNGIN_EAGAIN == ec) {
        if (session_.ev_.et())
          complete_ = true;
        break;
      }
      session_.flags_ |= flag_error;
      session_.last_error_ = ec;
      message_callback(in_buffer(buf), ec);
      break;
    }
    session_.flags_ &= ~flag_error;
    if (!size) {
      if (session_.ev_.et())
        complete_ = true;
      session_.flags_ |= flag_eof;
      message_callback(buf, KNGIN_EOF);
      break;
    } else {
      if (!buf.writeable()) {
        on_completion(true);
      } else {
        if (!need_complete)
          on_completion(false);
      }
    }
  } while (true);
}

void
posix_session::istream::on_oob () {
  assert(!session_.is_eof());
  do {
    if (session_.flags_ & (flag_closed | flag_closing))
      break;
    complete_ = false;

    uint8_t byte;
    in_buffer buf(&byte, 1);
    error_code ec;
    auto size = session_.socket_.recv(buf, socket::message_oob, ec);
    if (ec) {
      if (KNGIN_EINTR == ec)
        continue;
      if (KNGIN_EAGAIN == ec) {
        break;
      }
      session_.flags_ |= flag_error;
      session_.last_error_ = ec;
      message_callback(buf, ec);
      break;
    }
    session_.flags_ &= ~flag_error;
    if (!size) {
      if (session_.ev_.et())
        complete_ = true;
      session_.flags_ |= flag_eof;
      message_callback(buf, KNGIN_EOF);
      break;
    } else {
      TRY()
        oob_handler_(session_.owner_, byte, ec);
      IGNORE_EXCP("posix_session::istream::on_oob")
    }
  } while (true);
}

void
posix_session::istream::message_callback (const in_buffer &buf, const error_code &ec) {
  TRY ()
    message_handler_(session_.owner_, buf, ec);
  IGNORE_EXCP("posix_session::istream::message_callback")
}
void
posix_session::istream::on_completion(bool remove) {
  in_context ctx;
  {
    mutex::scoped_lock lock(ctxq_mutex_);
    ctx = ctxq_.back();
    if (remove) {
      ctxq_.pop_back();
      next_ctx_ = ctxq_.empty() ? nullptr : &ctxq_.back();
    }
  }
  message_callback(ctx.buffer, KNGIN_ESUCCESS);
}

} /* namespace tcp */

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_TCP_SESSION) */

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_TCP_SESSION)

#include "kngin/core/base/memory.h"
#include "kngin/core/base/exception.h"
#include "kngin/core/base/buffer.h"
#include "kngin/net/socket.h"
#include "detail/net/tcp/impl/posix_session.h"
#include "detail/net/tcp/impl/posix_tcp_istream.h"
#include "detail/net/tcp/impl/posix_tcp_ostream.h"

namespace k::detail::impl {

namespace tcp {

posix_session::posix_session (service &s, socket &&sock, k::tcp::session &owner,
                              write_handler &&o_cb, message_handler &&i_cb, oob_handler &&oob_cb)
 try
 : loop_(s.next_loop()),
   owner_(owner),
   socket_(std::move(sock)),
   addr_(),
   istream_(nullptr),
   ostream_(nullptr),
   ev_(socket_.handle(), [this] (event_loop &loop, int events) { on_events(loop, events); }),
   flags_(0) {
  socket_.set_nonblock(true);
  socket_.set_closeexec(true);
  socket_.peer_addr(addr_);
  istream_ = new istream(*this, socket_, std::move(i_cb), std::move(oob_cb));
  ostream_ = new ostream(*this, socket_, std::move(o_cb));
  ev_.enable_read();
  ev_.enable_write();
  ev_.enable_et();
  loop_.register_event(ev_);
} catch (...) {
  socket_.close();
  safe_release(istream_);
  safe_release(ostream_);
  throw;
}

posix_session::~posix_session () noexcept {
  TRY()
    if (!closed())
      close(nullptr);
    safe_release(istream_);
    safe_release(ostream_);
  IGNORE_EXCP()
}

bool
posix_session::async_write (const out_buffer &buf, int flags) {
  assert(!(flags_ & (flag_closed | flag_shutdown)));
  assert(buf.size());
  if (!(flags_ & (flag_closed | flag_shutdown)) and buf.size()) {
    ostream_->async_write(buf, flags);
    return true;
  }
  return false;
}

bool
posix_session::async_read (in_buffer &buf) {
  assert(!(flags_ & flag_closed));
  assert(buf.writeable());
  if (!(flags_ & flag_closed) and istream_ and buf.writeable()) {
    istream_->async_read(buf);
    return true;
  }
  return false;
}

bool
posix_session::async_read_some (in_buffer &buf) {
  assert(!(flags_ & flag_closed));
  assert(buf.writeable());
  if (!(flags_ & flag_closed) and buf.writeable()) {
    istream_->async_read_some(buf);
    return true;
  }
  return false;
}

void
posix_session::stop_read () {
  loop_.run_in_loop([this] () {
    if (ev_.out()) {
      ev_.disable_write();
      loop_.update_event(ev_);
    }
  });
}

void
posix_session::start_read () {
  loop_.run_in_loop([this] () {
    if (!ev_.out()) {
      ev_.enable_write();
      loop_.update_event(ev_);
    }
  });
}

void
posix_session::close (close_handler &&handler) {
  assert(handler);
  assert(!closed());
  if (!closed()) {
    loop_.run_in_loop([this, handler=std::move(handler)] () {
      flags_ |= flag_closed;
      flags_ |= flag_eof;
      loop_.remove_event(ev_);
      socket_.close();
      istream_->clear();
      ostream_->clear();
      TRY()
        if (handler)
          handler(this->owner_);
      CATCH_ERROR("posix_session::close_handler");
    });
  }
}

void
posix_session::shutdown () {
  assert(!closed());
  assert(is_shutdown());
  if (!closed() and !is_shutdown()) {
    flags_ |= flag_shutdown;
    socket_.shutdown();
  }
}

size_t
posix_session::remaining () const noexcept {
  return ostream_->remaining();
}

std::string
posix_session::ip_address () const {
  return addr_.ip_address();
}

uint16_t
posix_session::port () const noexcept {
  return addr_.port();
}

void
posix_session::on_events (event_loop &loop, int events) {
  assert(&loop_ == &loop);
  assert(!(events & ~(epoll_event::event_type_error
                      | epoll_event::event_type_read
                      | epoll_event::event_type_write
                      | epoll_event::event_type_oob)));

  TRY()
    if (events & (epoll_event::event_type_error | epoll_event::event_type_read)) {
      istream_->on_read();
    }
    if (events & (epoll_event::event_type_write)) {
      ostream_->on_write();
    }
    if (events & (epoll_event::event_type_oob)) {
      istream_->on_oob();
    }
  CATCH_ERROR("posix_listener::on_events")
}

} /* namespace tcp */

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_TCP_SESSION) */

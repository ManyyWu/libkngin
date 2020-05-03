#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_LISTENER)

#include "kngin/core/base/system_error.h"
#include "kngin/core/base/exception.h"
#include "detail/net/impl/posix_listener.h"
#include "kngin/net/socket.h"
#include "kngin/net/sockopts.h"

namespace k::detail::impl {

posix_listener::posix_listener (service &s, socket &sock, const address &addr,
                                int backlog, session_handler &&handler)
  try
 : loop_(s.next_loop()),
   socket_(sock),
   listen_addr_(addr),
   handler_(std::move(handler)),
   idle_file_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
   ev_(socket_.handle(), [this] (event_loop &loop, int events) { on_events(loop, events); }),
   flags_(0) {
  if (!handler_)
    throw_exception("invalid handler value");
  if (HANDLE_INVALID(idle_file_))
    throw_system_error("::open(\"dev/null\") error", last_error());
  sockopts::set_reuseaddr(sock, true);
  sockopts::set_reuseport(sock, true);
  socket_.set_nonblock(true);
  socket_.set_closeexec(true);
  descriptor::set_nonblock(idle_file_,true);
  descriptor::set_closeexec(idle_file_, true);
  sock.bind(addr);
  sock.listen(backlog);
  ev_.enable_read();
  ev_.enable_et();
  loop_.register_event(ev_);
} catch (...) {
  if (!sock.closed())
    sock.close();
  if (HANDLE_VALID(idle_file_))
    descriptor::close(idle_file_);
  throw;
}

posix_listener::~posix_listener () noexcept {
  TRY()
    if (!closed())
      close();
  IGNORE_EXCP("posix_listener::~posix_listener")
}

void
posix_listener::close () {
  assert(!closed());
  if (!closed()) {
    flags_ |= flag_closed;
    loop_.remove_event(ev_);
    socket_.close();
  }
}

void
posix_listener::on_events (event_loop &loop, int events) {
  assert(&loop_ == &loop);
  TRY()
    if (events & (epoll_event::event_type_error | epoll_event::event_type_read)) {
      on_read(loop);
    } else {
      assert(0);
    }
  CATCH_ERROR("posix_listener::on_events")
}

void
posix_listener::on_read (event_loop &loop) {
  assert(ev_.registed());
  assert(!closed());
  while (ev_.registed() and !closed()) {
    socket sock;
    address addr;
    error_code ec;
    socket_.accept(addr, sock, ec);
    if (ec) {
      switch (ec.value()) {
      case KNGIN_ENFILE:
      case KNGIN_EMFILE: // default: 1024
        {
          socket temp_sock;
          error_code temp_ec;
          descriptor::close(idle_file_, temp_ec);
          temp_sock.accept(addr, temp_sock, temp_ec);
          temp_sock.close(temp_ec);
          idle_file_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
          descriptor::close(idle_file_);
          log_error("the process already has the maximum number of files open, "
                    "a new session has been rejected");
        }
        continue;
      case KNGIN_EINTR:
        continue;
      case KNGIN_EAGAIN:
        return;
      case KNGIN_EPROTO:
      case KNGIN_ECONNABORTED:
        log_debug("listener ignored error: %s", ec.message());
        continue;
      default:
        log_error("socket::accept() error: ", ec.message());
        continue;
      }
    }

    TRY()
      handler_(std::move(sock), ec);
    IGNORE_EXCP("posix_listener::on_read");
  }
}

} /* namespace k::net::impl */

#endif /* defined(KNGIN_USE_POSIX_LISTENER) */

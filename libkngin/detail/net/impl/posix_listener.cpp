#include "kngin/core/base/system_error.h"
#include "kngin/core/base/exception.h"
#include "detail/net/impl/posix_listener.h"
#include "kngin/net/server_opts.h"
#include "kngin/net/socket.h"
#include "kngin/net/sockopts.h"

namespace k::detail::impl {

static
socket::inet_protocol
opt_to_protocol (const server_opts &opts) {
  if (opts.type & socket_type::tcp) {
    if (opts.domain & socket_domain::ipv6_only)
      return socket::ipv6_tcp;
    else
      return socket::ipv4_tcp;
  } else {
    if (opts.domain & socket_domain::ipv6_only)
      return socket::ipv6_udp;
    else
      return socket::ipv4_udp;
  }
}

posix_listener::posix_listener (service &s, const server_opts &opts, session_handler &&handler)
 : loop_(s.next_loop()),
   socket_(opt_to_protocol(opts)),
   listen_addr_(opts.name.c_str(), opts.port, opts.domain & socket_domain::ipv6_only),
   handler_(std::move(handler)),
   idle_file_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
   ev_(socket_.handle(),
       [this] (event_loop &loop, int events) { on_events(loop, events); }
       ) {
  if (!handler_)
    throw_exception("invalid handler value");
  if (idle_file_)
    throw_system_error("::open(\"dev/null\") error", last_error());
  ev_.enable_read();
  ev_.enable_et();
  loop_.register_event(ev_);
}

posix_listener::~posix_listener () noexcept {
  TRY()
    if (closed())
      close();
  IGNORE_EXCP()
}

void
posix_listener::close () {
  assert(closed());
  if (!closed()) {
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
  assert(closed());
  while (ev_.registed() and !closed()) {
    socket sock;
    address addr;
    error_code ec;
    socket_.accept(addr, sock, ec);
    if (ec) {
      if (EMFILE == ec) {
        socket temp_sock;
        error_code temp_ec;
        descriptor::close(idle_file_, temp_ec);
        temp_sock.accept(addr, temp_sock, temp_ec);
        temp_sock.close(temp_ec);
        idle_file_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        descriptor::close(idle_file_);
        log_error("the process already has the maximum number of files open, "
                  "a new session has been rejected");
      } else if (EINTR == ec) {
        continue;
      } else if (EAGAIN == ec or EPROTO == ec or ECONNABORTED == ec) {
        log_debug("listener ignored error: %s", ec.message().c_str());
      } else {
        log_error("socket::accept() error: ", ec.message().c_str());
      }
    }

    TRY()
      handler_(std::move(sock), ec);
    CATCH_ERROR("posix_listener::on_read");
  }
}

} /* namespace k::net::impl */

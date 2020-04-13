#include "kngin/core/base/memory.h"
#include "kngin/net/listener.h"
#include "kngin/net/service.h"
#include "kngin/net/server_opts.h"
#include "kngin/net/socket.h"
#include "detail/net/listener_impl.h"

namespace k {

listener::listener (service &s, const server_opts &opts, session_handler &&handler)
 : impl_(nullptr) {
  impl_ = new listener_impl(s, opts, std::move(handler));
}

listener::~listener () noexcept {
  safe_release(impl_);
}

void
listener::close () {
  impl_->close();
}

bool
listener::closed () const noexcept {
  return impl_->closed();
}

const address &
listener::listen_addr () const noexcept {
  return impl_->listen_addr();
}

} /* namespace k */

#include "kngin/core/base/memory.h"
#include "kngin/net/listener.h"
#include "kngin/net/service.h"
#include "kngin/net/socket.h"
#include "detail/net/listener_impl.h"

namespace k {

listener::listener (service &s, socket &sock, const address &addr,
                    int backlog, session_handler &&handler)
 : impl_(nullptr) {
  impl_ = new listener_impl(s, sock, addr, backlog, std::move(handler));
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

} /* namespace k */

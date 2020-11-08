#include "kngin/core/base/memory.h"
#include "kngin/net/connector.h"
#include "kngin/net/service.h"
#include "kngin/net/socket.h"
#include "detail/net/connector_impl.h"

namespace k {

connector::connector (service &s, inet_protocol proto, const address &addr)
 : impl_(nullptr) {
  impl_ = new connector_impl(s, proto, addr);
}

connector::~connector () {
  safe_release(impl_);
}

void
connector::async_connect (connect_handler &&handler) {
  impl_->async_connect(std::move(handler));
}

bool
connector::connected () const noexcept {
  return impl_->connected();
}

} /* namespace k */

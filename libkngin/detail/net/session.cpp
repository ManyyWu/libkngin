#include "kngin/core/base/memory.h"
#include "kngin/net/tcp/session.h"
#include "kngin/net/service.h"
#include "kngin/net/socket.h"
#include "detail/net/session_impl.h"

namespace k {

namespace tcp {

const in_buffer session::eof(nullptr, 0);

session::session (service &s, socket &&sock,
                  write_handler &&o_cb,
                  message_handler &&i_cb,
                  oob_handler &&oob_cb)
 : impl_(nullptr) {
  if (!o_cb or !i_cb or !oob_cb)
    throw_exception("invalid handler value");
  impl_ = new session_impl(s, std::move(sock), *this,
                           std::move(o_cb), std::move(i_cb), std::move(oob_cb));
}

session::~session () noexcept {
  safe_release(impl_);
}

bool
session::async_write (out_buffer buf) {
  return impl_->async_write(buf, 0);
}

bool
session::async_write (out_buffer buf, int flags) {
  return impl_->async_write(buf, flags);
}

bool
session::async_read (in_buffer buf) {
  return impl_->async_read(buf);
}

bool
session::async_read_some (in_buffer buf) {
  return impl_->async_read_some(buf);
}

void
session::close () {
  impl_->close();
}

void
session::shutdown () {
  impl_->shutdown();
}

bool
session::closed () const noexcept {
  return impl_->closed();
}

bool
session::is_shutdown () const noexcept {
  return impl_->is_shutdown();
}

bool
session::is_eof () const noexcept {
  return impl_->is_eof();
}

const error_code &
session::last_error () const noexcept {
  return impl_->last_error();
}

size_t
session::remaining () const noexcept {
  return impl_->remaining();
}

std::string
session::name () const {
  return impl_->name();
}

//const socket &
//session::get_socket () const noexcept {
//  return impl_->get_socket();
//}

void
session::broadcast (const session_list &list, out_buffer buf, int flags) {
  assert(list.size());
  assert(buf.size());

  for (auto &iter : list) {
    if (!iter->closed() and !iter->is_shutdown())
      iter->impl_->async_write(buf, flags);
  }
}

} /* namespace tcp */

} /* namespace k */
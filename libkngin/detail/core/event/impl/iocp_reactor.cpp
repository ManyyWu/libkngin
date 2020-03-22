#include "detail/core/event/impl/iocp_reactor.h"

#if defined(KNGIN_SYSTEM_WIN32)

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

iocp_reactor::iocp_reactor ()
 : iocp_(nullptr) {
  iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
  if (!iocp_)
    throw_system_error("::CreateIoCompletionPort() error", last_error());
}

iocp_reactor::~iocp_reactor () noexcept {
  TRY()
    this->close();
  IGNORE_EXCP()
}

size_t
iocp_reactor::wait (op_queue &ops, timestamp delay) {
}

void
iocp_reactor::wakeup () {
  if (::PostQueuedCompletionStatus(iocp_, 0, 0, nullptr))
    throw_system_error("::PostQueuedCompletionStatus() error", last_error());
}

void
iocp_reactor::close () {
  if (iocp_) {
    ::CloseHandle(iocp_);
    iocp_ = nullptr;
  }
}

void
iocp_reactor::register_event (class iocp_event &ev) {
  assert(iocp_);
  iocp_ = ::CreateIoCompletionPort(ev.handle(), iocp_, &ev, 1);
  if (!iocp_)
    throw_system_error("::CreateIoCompletionPort() error", last_error());
}

void
iocp_reactor::remove_event (class iocp_event &ev) {
  assert(iocp_);

}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_SYSTEM_WIN32) */

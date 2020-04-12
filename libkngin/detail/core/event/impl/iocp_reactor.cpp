#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_REACTOR)

#include "detail/core/event/impl/iocp_reactor.h"

namespace k::detail::impl {

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
  return (g_have_get_iocp_status_ex ? poll(ops, delay) : poll_wine(ops, delay));
}

size_t
iocp_reactor::poll (op_queue &ops, const timestamp &_ms)
{
  ULONG            size = 0;
  ULONG            count = 0;
  ULONG_PTR        key = 0;
  OVERLAPPED_ENTRY overlappeds[KNGIN_IOCP_REACTOR_MAX_EVENTS];

  ::GetQueuedCompletionStatusEx(iocp_,
                                overlappeds,
                                KNGIN_IOCP_REACTOR_MAX_EVENTS,
                                &size,
                                _ms.value_uint(),
                                FALSE);
  auto ec = last_error();
  if (size > 0) {
    count = size;
    for (size_t i = 0; i < size; ++i) {
      auto overlapped = overlappeds[i].lpOverlapped;
      if (overlapped) {
        ops.push(*(operation_base *)CONTAINING_RECORD(
            &overlappeds[i], iocp_operation, overlapped_));
      } else {
        --count; // wakeup;
      }
    }
  } else if (WAIT_TIMEOUT == ec) {
    return 0;
  } else {
    throw_system_error("::GetQueuedCompletionStatusEx() error", ec);
  }
  return count;
}

size_t
iocp_reactor::poll_wine (op_queue &ops, const timestamp &_ms)
{
  auto   size = KNGIN_IOCP_REACTOR_MAX_EVENTS;
  size_t count = 0;
  while (count < size) {
    DWORD       bytes = 0;
    ULONG_PTR   key = 0;
    OVERLAPPED *overlapped = nullptr;

    bool _ok = ::GetQueuedCompletionStatus(iocp_,
                                           &bytes,
                                           &key,
                                           &overlapped,
                                           _ms.value_uint());
    auto ec = last_error();
    if (_ok) {
      if (overlapped)
        ops.push(*(operation_base *)CONTAINING_RECORD(
            overlapped, iocp_operation, overlapped_));
      else
        break; // wakeup
    } else if (WAIT_TIMEOUT == ec) {
      return 0;
    } else {
      throw_system_error("::GetQueuedCompletionStatus() error", ec);
    }
  }
  return count;
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
  iocp_ = ::CreateIoCompletionPort(ev.handle(), iocp_, ULONG_PTR(&ev), 1);
  if (!iocp_)
    throw_system_error("::CreateIoCompletionPort() error", last_error());
  ev.set_registed(true);
}

void
iocp_reactor::remove_event (class iocp_event &ev) {
  assert(iocp_);
  ev.cancel();
  ev.set_registed(false);
}

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

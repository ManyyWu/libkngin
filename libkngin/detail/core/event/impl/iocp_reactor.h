#ifndef KNGIN_IOCP_REACTOR_H
#define KNGIN_IOCP_REACTOR_H

#include "kngin/core/define.h"
#if defined(KNGIN_SYSTEM_WIN32)

#include "detail/core/event/op_queue.h"
#include "detail/core/base/win_utils.h"
#include "detail/core/event/impl/iocp_event.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class iocp_reactor {
public:
  iocp_reactor ();

  ~iocp_reactor () noexcept;

  size_t
  wait (op_queue &ops, timestamp delay);

  void
  wakeup ();

  void
  close ();

  void
  register_event (class iocp_event &ev);

  void
  remove_event (class iocp_event &ev);

private:
  HANDLE iocp_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_IOCP_REACTOR_H */

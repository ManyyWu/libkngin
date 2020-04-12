#ifndef KNGIN_IOCP_REACTOR_H
#define KNGIN_IOCP_REACTOR_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_REACTOR)

#include "detail/core/base/win_utils.h"
#include "detail/core/event/impl/iocp_event.h"

namespace k::detail::impl {

class iocp_reactor : public noncopyable {
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
  size_t
  poll (op_queue &ops, const timestamp &_ms);

  size_t
  poll_wine (op_queue &ops, const timestamp &_ms);

private:
  handle_t iocp_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

#endif /* KNGIN_IOCP_REACTOR_H */

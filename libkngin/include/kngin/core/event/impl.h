#ifndef KNGIN_EVENT_IMPL_H
#define KNGIN_EVENT_IMPL_H

#include "kngin/core/define.h"

// reactor
#if defined(KNGIN_USE_IOCP_REACTOR)
  declare_detail_impl_class(iocp_reactor)
  typedef_detail_impl(iocp_reactor, reactor)
#elif defined(KNGIN_USE_EPOLL_REACTOR)
  declare_detail_impl_class(epoll_reactor)
  typedef_detail_impl(epoll_reactor, reactor)
#elif defined(KNGIN_USE_POLL_REACTOR)
  declare_detail_impl_class(poll_reactor)
  typedef_detail_impl(poll_reactor, reactor)
#elif defined(KNGIN_USE_SELECT_REACTOR)
  declare_detail_impl_class(select_reactor)
  typedef_detail_impl(select_reactor, reactor)
#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

// timer
#if defined(KNGIN_USE_TIMERFD_TIMER)
declare_detail_impl_class(timerfd_timer);
  typedef_detail_impl(timerfd_timer, timer);
#elif defined(KNGIN_USE_MONOTONIC_TIMER)
declare_detail_impl_class(monotonic_timer);
typedef_detail_impl(monotonic_timer, timer);
#elif defined(KNGIN_USE_QUEUED_TIMER)
declare_detail_impl_class(queued_timer);
  typedef_detail_impl(queued_timer, timer);
#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */

#endif /* KNGIN_EVENT_IMPL_H */

#ifndef KNGIN_REACTOR_IMPL_H
#define KNGIN_REACTOR_IMPL_H

#include "kngin/core/define.h"

#if defined(KNGIN_USE_IOCP_REACTOR)
#  include "kngin/core/event/impl/iocp_reactor.h"
  typedef iocp_reactor reactor;
#elif defined(KNGIN_USE_EPOLL_REACTOR)
#  include "kngin/core/event/impl/epoll_reactor.h"
  typedef epoll_reactor reactor;
#elif defined(KNGIN_USE_POLL_REACTOR)
#  include "kngin/core/event/impl/poll_reactor.h"
  typedef poll_reactor reactor;
#elif defined(KNGIN_USE_SELECT_REACTOR)
#  include "kngin/core/event/impl/select_reactor.h"
  typedef select_reactor reactor;
#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

#endif /* KNGIN_REACTOR_IMPL_H */
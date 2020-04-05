#ifndef KNGIN_REACTOR_IMPL_H
#define KNGIN_REACTOR_IMPL_H

#include "kngin/core/define.h"

#if defined(KNGIN_USE_IOCP_REACTOR)
#  include "detail/core/event/impl/iocp_reactor.h"

#elif defined(KNGIN_USE_EPOLL_REACTOR)
#  include "detail/core/event/impl/epoll_reactor.h"
#elif defined(KNGIN_USE_POLL_REACTOR)
#  include "detail/core/event/impl/poll_reactor.h"
#elif defined(KNGIN_USE_SELECT_REACTOR)
#  include "detail/core/event/impl/select_reactor.h"
#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

#endif /* KNGIN_REACTOR_IMPL_H */
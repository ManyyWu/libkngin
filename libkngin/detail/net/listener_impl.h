#ifndef KNGIN_LISTENER_IMPL_H
#define KNGIN_LISTENER_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_LISTENER)
#  include "detail/net/impl/iocp_listener.h"
#elif defined(KNGIN_USE_POSIX_LISTENER)
#  include "detail/net/impl/posix_listener.h"
#endif /* defined(KNGIN_USE_WIN_LISTENER) */

#endif /* KNGIN_LISTENER_IMPL_H */

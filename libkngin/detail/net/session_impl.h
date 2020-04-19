#ifndef KNGIN_TCP_SESSION_IMPL_H
#define KNGIN_TCP_SESSION_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_TCP_SESSION)
#  include "detail/net/tcp/impl/iocp_session.h"
#elif defined(KNGIN_USE_POSIX_TCP_SESSION)
#  include "detail/net/tcp/impl/posix_session.h"
#endif /* defined(KNGIN_USE_WIN_TCP_SESSION) */

#endif /* KNGIN_TCP_SESSION_IMPL_H */

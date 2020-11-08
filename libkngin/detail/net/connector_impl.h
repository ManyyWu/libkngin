#ifndef KNGIN_CONNECTOR_IMPL_H
#define KNGIN_CONNECTOR_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_CONNECTOR)
#  include "detail/net/impl/iocp_connector.h"
#elif defined(KNGIN_USE_POSIX_CONNECTOR)
#  include "detail/net/impl/posix_connector.h"
#endif /* defined(KNGIN_USE_WIN_LISTENER) */

#endif /* KNGIN_CONNECTOR_IMPL_H */

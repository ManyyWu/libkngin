#ifndef KNGIN_NET_IMPL_H
#define KNGIN_NET_IMPL_H

#include "kngin/core/define.h"

// listener
#if defined(KNGIN_USE_POSIX_LISTENER)
  declare_detail_impl_class(posix_listener)
  typedef_detail_impl(posix_listener, listener_impl)
#elif defined(KNGIN_USE_IOCP_LISTENER)
  declare_detail_impl_class(iocp_listener)
  typedef_detail_impl(iocp_listener, listener_impl)
#endif /* defined(KNGIN_USE_POSIX_LISTENER) */

// session
#if defined(KNGIN_USE_POSIX_TCP_SESSION)
  declare_detail_namespace_impl_class(tcp, posix_session)
  typedef_detail_namespace_impl(tcp, posix_session, session_impl)
#elif defined(KNGIN_USE_IOCP_TCP_SESSION)
  declare_detail_impl_class(iocp_session)
  typedef_detail_impl(iocp_session, session_impl)
#endif /* defined(KNGIN_USE_POSIX_TCP_SESSION) */

// connector
#if defined(KNGIN_USE_POSIX_CONNECTOR)
  declare_detail_impl_class(posix_connector)
  typedef_detail_impl(posix_connector, connector_impl)
#elif defined(KNGIN_USE_IOCP_CONNECTOR)
  declare_detail_impl_class(iocp_connector)
  typedef_detail_impl(iocp_connector, connector_impl)
#endif /* defined(KNGIN_USE_POSIX_CONNECTOR) */

#endif /* KNGIN_NET_IMPL_H */
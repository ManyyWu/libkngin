#ifndef KNGIN_IOCP_OPERATION_H
#define KNGIN_IOCP_OPERATION_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_REACTOR)

#include "kngin/core/base/noncopyable.h"

namespace k::detail::impl {

class event_loop;
class iocp_operation
  : public noncoypable,
    public operation_base {
public:
  friend class iocp_reactor;

protected:
  OVERLAPPED overlapped_;

  WSABUF data_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

#endif /* KNGIN_IOCP_OPERATION_H */
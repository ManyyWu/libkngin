#ifndef KNGIN_SOCKET_H
#define KNGIN_SOCKET_H

#include "kngin/core/define.h"

KNGIN_NAMESPACE_K_BEGIN

class socket {
public:
  handle_t
  handle () const noexcept {
    return handle_;
  }

  handle_t handle_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SOCKET_H */

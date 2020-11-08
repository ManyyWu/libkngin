#ifndef KNGIN_POSIX_CONNECTOR_H
#define KNGIN_POSIX_CONNECTOR_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_CONNECTOR)

#include "kngin/net/detail.h"
#include "kngin/net/connector.h"
#include "kngin/net/service.h"

namespace k::detail::impl {

class posix_connector {
public:
  typedef connector::inet_protocol inet_protocol;
  typedef connector::connect_handler connect_handler;

  posix_connector (service &s, inet_protocol proto, const address &addr);

  ~posix_connector () noexcept;

  void
  async_connect (connect_handler &&handler);

  bool
  connected () const noexcept {
    return (state_ & state_connected);
  }

private:
  void
  on_events (event_loop &loop, int events);

  void
  on_write ();

private:
  enum {
    state_connecting = 0x01,
    state_connected  = 0x02,
    state_error      = 0x04,
  };

  event_loop &loop_;

  handle_t socket_;

  address addr_;

  connect_handler handler_;

  reactor_event ev_;

  std::atomic_int state_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_CONNECTOR) */

#endif /* KNGIN_POSIX_CONNECTOR_H */
#ifndef KNGIN_CONNECTOR_H
#define KNGIN_CONNECTOR_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/net/socket.h"
#include "kngin/net/detail.h"
#include <functional>

namespace k {

class address;
class service;
class connector : public noncopyable {
public:
  typedef socket::inet_protocol inet_protocol;
  typedef std::function<void (socket &, error_code)> connect_handler;

  connector (service &s, inet_protocol proto, const address &addr);

  ~connector () noexcept;

  void
  async_connect (connect_handler &&handler);

  bool
  connected () const noexcept;

private:
  void
  connect (error_code &ec) noexcept;

private:
  connector_impl *impl_;
};

} /* namespace k */

#endif /* KNGIN_CONNECTOR_H */

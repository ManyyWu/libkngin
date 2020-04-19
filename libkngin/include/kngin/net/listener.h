#ifndef KNGIN_LISTENR_H
#define KNGIN_LISTENR_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/net/detail.h"
#include <functional>

namespace k {

class address;
class socket;
class service;
class listener : public noncopyable {
public:
  typedef std::function<void (socket &&, error_code &ec)> session_handler;

  listener (service &s, socket &sock, const address &addr,
            int backlog, session_handler &&handler);

  ~listener () noexcept;

  void
  close ();

  bool
  closed () const noexcept;

private:
  listener_impl *impl_;
};

} /* namespace k */

#endif /* KNGIN_LISTENR_H */

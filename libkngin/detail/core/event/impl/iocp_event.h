#ifndef KNGIN_IOCP_EVENT_H
#define KNGIN_IOCP_EVENT_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_REACTOR)

#include "kngin/core/base/noncopyable.h"
#include "detail/core/base/win_utils.h"
#include "detail/core/event/impl/iocp_operation.h"

namespace k::detail::impl {

class iocp_event : public noncopyable {
  friend class iocp_reactor;

public:
  enum {
    handle_type_unknown = 0,
    handle_type_file    = 1,
    handle_type_socket  = 2
  };

  enum {
    event_type_read  = 1,
    event_type_write = 2,
    event_type_error = 4,
    event_type_oob   = 8,
  };

  typedef std::function<void (event_loop &, int)> handler;

  iocp_event () noexcept
   : handle_(INVALID_HANDLE),
     type_(handle_type_unknown),
     registed_(false),
     handler_() {
  }

  explicit
  iocp_event (handle_t h, handle_type type, handler cb)
   : handle_(h),
     type_(type),
     registed_(false),
     handler_(std::move(cb)) {
  }

  iocp_event (iocp_event &&ev) noexcept
   : handle_(nullptr),
     type_(handle_type_unknown),
     registed_(false),
     handler_() {
    std::swap(handle_, ev.handle_);
    std::swap(type_, ev.type_);
    std::swap(registed_, ev.registed_);
    //std::swap(handler_, ev.handler_) // unsafe
  }

  virtual
  ~iocp_event() noexcept {
  }

  void
  set_handler (handler &&cb) noexcept {
    handler_ = std::move(cb);
  }

  handle_t
  handle () const noexcept {
    return handle_;
  }

  bool
  registed () const noexcept {
    return registed_;
  }

  handle_type
  type () const noexcept {
    return type_;
  }

private:
  void
  set_registed (bool on) noexcept {
    registed_ = on;
  }

  void
  on_events (event_loop &loop, int events) {
    TRY()
      if (handler_)
        handler_(loop, events);
    CATCH_ERROR("iocp_event::on_events")
  }

protected:
  handle_t handle_;

  handle_type type_;

private:
  bool registed_;

  handler handler_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

#endif /* KNGIN_IOCP_EVENT_H */
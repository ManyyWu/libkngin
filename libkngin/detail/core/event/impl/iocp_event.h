#ifndef KNGIN_IOCP_EVENT_H
#define KNGIN_IOCP_EVENT_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_IOCP_REACTOR)

#include "detail/core/base/win_utils.h"
#include "detail/core/event/impl/iocp_operation.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class iocp_event {
  friend class iocp_reactor;

public:
  enum handle_type {
    handle_type_unknown = 0,
    handle_type_file    = 1,
    handle_type_socket  = 2
  };

  iocp_event () = delete;

  explicit
  iocp_event (handle_t h, handle_type type)
   : handle_(h),
     type_(type) {
  }

  iocp_event (iocp_event &&ev) noexcept
   : handle_(nullptr),
     type_(handle_type_unknown),
     registed_(false) {
    std::swap(handle_, ev.handle_);
    std::swap(type_, ev.type_);
    std::swap(registed_, ev.registed_);
  }

  virtual
  ~iocp_event() noexcept {
  }

  bool
  registed () const noexcept {
    return registed_;
  }

  handle_type
  type () const noexcept {
    return type_;
  }

protected:
  typedef operation_base::op_type op_type;

  handle_t
  handle () const noexcept {
    return handle_;
  }

  virtual
  op_queue *
  get_op_queue (op_type) noexcept {
    return nullptr;
  }

  virtual
  void
  cancel () = 0;

private:
  void
  set_registed (bool on) noexcept {
    registed_ = on;
  }

protected:
  handle_t handle_;

  handle_type type_;

private:
  bool registed_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

#endif /* KNGIN_IOCP_EVENT_H */
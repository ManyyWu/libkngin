#ifndef KNGIN_OPERATION_BASE_H
#define KNGIN_OPERATION_BASE_H

#include "kngin/core/define.h"
#include "kngin/core/event/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop;
class operation_base {
public:
  typedef enum class : unsigned char {
    op_read  = 0,
    op_write = 1,
    op_error = 2
  } op_type;

  explicit
  operation_base (reactor_event &owner, op_type type) noexcept
   : owner_(owner),
     type_(type) {
  }

  virtual
  ~operation_base () noexcept {
  }

  op_type
  type () const noexcept {
    return type_;
  }

  virtual
  reactor_event &
  owner () noexcept {
    return owner_;
  }

  virtual
  void
  on_operation (event_loop &) = 0;

protected:
  reactor_event &owner_;

  op_type type_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_OPERATION_BASE_H */
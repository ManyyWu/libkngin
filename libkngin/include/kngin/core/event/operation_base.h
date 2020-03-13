#ifndef KNGIN_OPERATION_BASE_H
#define KNGIN_OPERATION_BASE_H

#include "kngin/core/define.h"
#include "detail/core/base/list_node.h"

KNGIN_NAMESPACE_K_BEGIN

class operation_base : public detail::entry_base<operation_base> {
public:
  enum class op_type : unsigned char {
    op_read  = 0,
    op_write = 1,
    op_error = 2
  };

  explicit
  operation_base (op_type type) noexcept
   : type_(type) {
  }

  virtual
  ~operation_base () {
  }

  op_type
  type () const noexcept {
    return type_;
  }

  virtual
  void
  on_operation () = 0;

private:
  op_type type_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_OPERATION_BASE_H */
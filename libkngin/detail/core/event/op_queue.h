#ifndef KNGIN_OP_QUEUE_H
#define KNGIN_OP_QUEUE_H

#include "kngin/core/define.h"
#include "kngin/core/event/operation_base.h"
#include <vector>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

class op_queue {
public:
  typedef operation_base::op_type op_type;

  op_queue ()
   : opq_() {
  }

  virtual
  ~op_queue () noexcept {
  }

  virtual
  void
  push (operation_base &op) = 0;

  virtual
  void
  pop () = 0;

  virtual
  operation_base &
  top () = 0;

  virtual
  size_t
  size () const noexcept = 0;

  virtual
  bool
  empty () const noexcept = 0;

  virtual
  void
  clear () = 0;

protected:
  typedef std::vector<operation_base *> queue_type;

  queue_type opq_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_OP_QUEUE_H */
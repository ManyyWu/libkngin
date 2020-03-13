#ifndef KNGIN_OP_QUEUE_H
#define KNGIN_OP_QUEUE_H

#include "kngin/core/define.h"
#include "kngin/core/base/exception.h"
#include "kngin/core/event/operation_base.h"
#include <vector>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

//template<>
class op_queue {
public:
  op_queue ()
   : opq_() {
  }

  ~op_queue () {
    clear();
  }

  void
  push (operation_base &op) {
    opq_.push_back(&op);
  }

  void
  pop () {
    if (opq_.empty())
      throw_exception("out of range");
    opq_.pop_back();
  }

  operation_base &
  top () {
    if (opq_.empty())
      throw_exception("out of range");
    return *opq_.back();
  }

  void
  clear () {
    opq_.clear();
  }

private:
  typedef std::vector<operation_base *> queue_type;

  queue_type opq_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_OP_QUEUE_H */
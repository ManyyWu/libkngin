/*
class posix_socket::socket_op_queue : public op_queue {
public:
  typedef operation_base::op_type op_type;

  socket_op_queue ()
   : op_queue(),
     mutex_() {
  }

  virtual
  ~socket_op_queue () noexcept {
    this->clear();
  }

  virtual
  void
  push (operation_base &op) {
    mutex::scoped_lock lock(mutex_);
    opq_.push_back(&op);
  }

  virtual
  void
  pop () {
    mutex::scoped_lock lock(mutex_);
    if (opq_.empty())
      throw_exception("out of range");
    opq_.pop_back();
  }

  virtual
  operation_base &
  top () {
    mutex::scoped_lock lock(mutex_);
    if (opq_.empty())
      throw_exception("out of range");
    return *opq_.back();
  }

  virtual
  void
  clear () {
    mutex::scoped_lock lock(mutex_);
    opq_.clear();
  }

private:
  mutex mutex_;
};
*/
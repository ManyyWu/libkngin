#ifndef KNGIN_EPOLL_EVENT_H
#define KNGIN_EPOLL_EVENT_H

#include "kngin/core/define.h"

#if !defined(KNGIN_SYSTEM_WIN32)
#include "kngin/core/event/event_base.h"
#include "detail/core/event/op_queue.h"
#include <sys/epoll.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class epoll_reactor;
class epoll_event : public entry_base<epoll_event> {
  friend class epoll_reactor;
public:
  epoll_event () = delete;

  explicit
  epoll_event (int fd) noexcept
   : fd_(fd),
     flags_(EPOLLERR | EPOLLHUP),
     registed_(false) {
  }

  epoll_event (class epoll_event &&e) noexcept {
    std::swap(fd_, e.fd_);
    std::swap(flags_, e.flags_);
    std::swap(registed_, e.registed_);
  }

  virtual
  ~epoll_event () noexcept {
  }

  bool
  registed () const noexcept {
    return registed_;
  }

protected:
  int
  fd () const noexcept {
    return fd_;
  }

  void
  set_flags (uint32_t flags) noexcept {
    flags_ = flags;
  }
  uint32_t
  flags () const noexcept {
    return flags_;
  }
  void
  enable_read () noexcept {
    flags_ |= EPOLLIN;
  }
  void
  enable_write () noexcept {
    flags_ |= EPOLLOUT;
  }
  void
  enable_oob () noexcept {
    flags_ |= EPOLLPRI;
  }
  void
  enable_once () noexcept {
    flags_ |= EPOLLONESHOT;
  }
  void
  enable_et () noexcept {
    flags_ |= EPOLLET;
  }
  void
  disable_read () noexcept {
    flags_ &= ~EPOLLIN;
  }
  void
  disable_write () noexcept {
    flags_ &= ~EPOLLOUT;
  }
  void
  disable_oob () noexcept {
    flags_ &= ~EPOLLPRI;
  }
  void
  disable_once () noexcept {
    flags_ &= ~EPOLLONESHOT;
  }
  void
  disable_et () noexcept {
    flags_ &= ~EPOLLET;
  }
  bool
  pollin () const noexcept {
    return (flags_ & EPOLLIN);
  }
  bool
  pollout () const noexcept {
    return (flags_ & EPOLLOUT);
  }
  bool
  pollpri () const noexcept {
    return (flags_ & EPOLLPRI);
  }
  bool
  pollonce () const noexcept {
    return (flags_ & EPOLLONESHOT);
  }
  bool
  et () const noexcept {
    return (flags_ & EPOLLET);
  }

  op_queue &
  read_op_queue () noexcept {
    return opq_[0];
  }
  op_queue &
  write_op_queue () noexcept {
    return opq_[1];
  }
  op_queue &
  error_op_queue () noexcept {
    return opq_[0];
  }

private:
  void
  set_registed (bool on) noexcept {
    registed_ = on;
  }

private:
  int fd_;

  uint32_t flags_;

  bool registed_;

  op_queue opq_[2];
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_EPOLL_EVENT_H */
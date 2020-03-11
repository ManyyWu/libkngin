#ifndef KNGIN_EPOLL_EVENT_H
#define KNGIN_EPOLL_EVENT_H

#include "kngin/core/define.h"

#if !defined(KNGIN_SYSTEM_WIN32)
#include "kngin/core/event/event_base.h"
#include "detail/core/base/list_node.h"
#include <sys/epoll.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class event_loop;
class epoll_event : entry_base<epoll_event> {
public:
  epoll_event () = delete;

  explicit
  epoll_event (int fd) noexcept
   : fd_(fd),
     flags_(EPOLLERR | EPOLLHUP),
     event_({0, nullptr}),
     registed_(false) {
  }

  epoll_event (class epoll_event &&e) noexcept {
    std::swap(fd_, e.fd_);
    std::swap(flags_, e.flags_);
    std::swap(event_, e.event_);
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
  void
  set_registed (bool on) noexcept {
    registed_ = on;
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
  void
  disable_all () noexcept {
    flags_ = EPOLLHUP | EPOLLERR;
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

  virtual
  void
  on_events (event_loop &loop, uint32_t flags) = 0;

private:
  int fd_;

  uint32_t flags_;

  struct ::epoll_event event_;

  bool registed_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_EPOLL_EVENT_H */
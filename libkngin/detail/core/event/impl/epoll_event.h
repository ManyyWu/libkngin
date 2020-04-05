#ifndef KNGIN_EPOLL_EVENT_H
#define KNGIN_EPOLL_EVENT_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_EPOLL_REACTOR)

#include "kngin/core/base/common.h"
namespace sys::epoll {
#include <sys/epoll.h>
}
#include <functional>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class epoll_reactor;
class epoll_event : public noncopyable {
  friend class epoll_reactor;
  friend class k::event_loop;

public:
  enum {
    event_type_read  = 1,
    event_type_write = 2,
    event_type_error = 4,
    event_type_oob   = 8,
  };

  typedef std::function<void (event_loop &, int)> handler;

  epoll_event () noexcept
   : handle_(INVALID_HANDLE),
     flags_(sys::epoll::EPOLLERR | sys::epoll::EPOLLHUP),
     registed_(false),
     handler_() {
  }

  explicit
  epoll_event (handle_t h, handler &&cb) noexcept
   : handle_(h),
     flags_(sys::epoll::EPOLLERR | sys::epoll::EPOLLHUP),
     registed_(false),
     handler_(std::move(cb)) {
    }

  epoll_event (epoll_event &&ev) noexcept
    : handle_(INVALID_HANDLE),
      flags_(sys::epoll::EPOLLERR | sys::epoll::EPOLLHUP),
      registed_(false),
      handler_() {
    std::swap(handle_, ev.handle_);
    std::swap(flags_, ev.flags_);
    std::swap(registed_, ev.registed_);
    //std::swap(handler_, ev.handler_); // unsafe
  }

  ~epoll_event () noexcept {
  }

  void
  set_handler (handler &&cb) noexcept {
    handler_ = std::move(cb);
  }

  bool
  registed () const noexcept {
    return registed_;
  }

  handle_t
  handle () const noexcept {
    return handle_;
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
    flags_ |= sys::epoll::EPOLLIN;
  }
  void
  enable_write () noexcept {
    flags_ |= sys::epoll::EPOLLOUT;
  }
  void
  enable_oob () noexcept {
    flags_ |= sys::epoll::EPOLLPRI;
  }
  void
  enable_once () noexcept {
    flags_ |= sys::epoll::EPOLLONESHOT;
  }
  void
  enable_et () noexcept {
    flags_ |= sys::epoll::EPOLLET;
  }
  void
  disable_read () noexcept {
    flags_ &= ~sys::epoll::EPOLLIN;
  }
  void
  disable_write () noexcept {
    flags_ &= ~sys::epoll::EPOLLOUT;
  }
  void
  disable_oob () noexcept {
    flags_ &= ~sys::epoll::EPOLLPRI;
  }
  void
  disable_once () noexcept {
    flags_ &= ~sys::epoll::EPOLLONESHOT;
  }
  void
  disable_et () noexcept {
    flags_ &= ~sys::epoll::EPOLLET;
  }
  bool
  pollin () const noexcept {
    return (flags_ & sys::epoll::EPOLLIN);
  }
  bool
  pollout () const noexcept {
    return (flags_ & sys::epoll::EPOLLOUT);
  }
  bool
  pollpri () const noexcept {
    return (flags_ & sys::epoll::EPOLLPRI);
  }
  bool
  pollonce () const noexcept {
    return (flags_ & sys::epoll::EPOLLONESHOT);
  }
  bool
  et () const noexcept {
    return (flags_ & sys::epoll::EPOLLET);
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
    CATCH_ERROR("epoll_event::on_events")
  }

protected:
  handle_t handle_;

private:
  uint32_t flags_;

  bool registed_;

  handler handler_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_EPOLL_REACTOR) */

#endif /* KNGIN_EPOLL_EVENT_H */
#include "kngin/core/define.h"
#if defined(KNGIN_USE_EPOLL_REACTOR)

#include "detail/core/base/descriptor.h"
#include "detail/core/event/impl/epoll_reactor.h"
#include <sys/eventfd.h>

namespace k::detail::impl {

epoll_reactor::epoll_reactor ()
 : epoll_fd_(::sys::epoll::epoll_create1(sys::epoll::EPOLL_CLOEXEC)),
   waker_fd_(::eventfd(0, EFD_CLOEXEC)) {
  if (epoll_fd_ < 0)
    throw_system_error("::sys::epoll::epoll_create1() error", last_error());
  if (waker_fd_ < 0) {
    ::close(epoll_fd_);
    throw_system_error("::eventfd() error", last_error());
  }

  // add waker to epoll
  sys::epoll::epoll_event ev = {sys::epoll::EPOLLHUP
                                  | sys::epoll::EPOLLERR
                                  | sys::epoll::EPOLLET
                                  | sys::epoll::EPOLLIN,
                                this};
  if (sys::epoll::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, waker_fd_, &ev) < 0)
    throw_system_error("::epoll_ctl() error", last_error());
}

epoll_reactor::~epoll_reactor () noexcept {
  TRY()
    close();
  IGNORE_EXCP("epoll_reactor::~epoll_reactor")
}

size_t
epoll_reactor::wait (event_list &list, timestamp delay) {
  sys::epoll::epoll_event internal_events[KNGIN_EPOLL_REACTOR_MAX_EVENTS];
  auto size = sys::epoll::epoll_wait(epoll_fd_, internal_events,
                           KNGIN_EPOLL_REACTOR_MAX_EVENTS, delay.value_int());
  auto ec = last_error();
  if (size < 0) {
    if (KNGIN_EINTR != ec)
      throw_system_error("::epoll_wait() error", last_error());
    return 0;
  }
  for (int i = 0; i < size; ++i) {
    auto &internal_event = internal_events[i];
    if (internal_event.data.ptr == this) {
      on_wakeup();
      continue;
    }
    auto *event = static_cast<epoll_event *>(internal_event.data.ptr);
    assert(event);
    if (internal_event.events & (sys::epoll::EPOLLERR | sys::epoll::EPOLLHUP)) {
      list.push_back({event, epoll_event::event_type_error});
      continue;
    }
    int flags = 0;
    if (internal_event.events & sys::epoll::EPOLLIN)
      flags |= epoll_event::event_type_read;
    if (internal_event.events & sys::epoll::EPOLLOUT)
      flags |= epoll_event::event_type_write;
    if (internal_event.events & sys::epoll::EPOLLPRI)
      flags |= epoll_event::event_type_oob;
    list.push_back({event, flags});
  }
  return std::max<size_t>(size, 0);
}

void
epoll_reactor::wakeup () {
  int64_t val = 1;
  auto buf = out_buffer(&val, 8);
  descriptor::write(waker_fd_, buf);
}

void
epoll_reactor::close () {
  if (HANDLE_VALID(epoll_fd_))
    descriptor::close(epoll_fd_);
  if (HANDLE_VALID(waker_fd_))
    descriptor::close(waker_fd_);
}

void
epoll_reactor::register_event (epoll_event &ev) {
  assert(!ev.registed());
  ev.set_registed(true);
  update_event(EPOLL_CTL_ADD, ev.handle(), &ev);
}

void
epoll_reactor::remove_event (epoll_event &ev) {
  assert(ev.registed());
  ev.set_registed(false);
  update_event(EPOLL_CTL_DEL, ev.handle(), &ev);
}

void
epoll_reactor::modify_event (epoll_event &ev) {
  assert(ev.registed());
  update_event(EPOLL_CTL_MOD, ev.handle(), &ev);
}

void
epoll_reactor::update_event (int opt, handle_t h, epoll_event *ev) {
  /**
   * NOTES:
   * While one thread is blocked in a call to epoll_pwait(), it is possible for another thread
   * to add a file descriptor to the waited-upon epoller instance. If the new file descriptor
   * becomes ready, it will cause the epoll_wait() call to unblock.
   *
   * If a file descriptor being monitored by select() is closed in another thread, the result
   * is unspecified.  On some UNIX systems, select() unblocks and returns, with an indication
   * that the file descriptor is ready (a subsequent I/O operation will likely fail with an error,
   * unless another the file descriptor reopened between the time select() returned  and the I/O
   * operations was performed).  On Linux (and some other systems), closing the file descriptor
   * in another thread has no effect on select().  In summary, any application that relies on a
   * particular behavior in this scenario must be considered buggy.
   */

  sys::epoll::epoll_event internal_event = {ev->flags(), ev};
  if (sys::epoll::epoll_ctl(epoll_fd_, opt, h, &internal_event) < 0)
    throw_system_error("::epoll_ctl() error", KNGIN_ERRNO(errno));
}

void
epoll_reactor::on_wakeup () {
  int64_t val = 0;
  auto buf = in_buffer(&val, 8);
  descriptor::read(waker_fd_, buf);
}

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_EPOLL_REACTOR) */

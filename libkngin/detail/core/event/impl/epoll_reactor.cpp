#include "detail/core/event/impl/epoll_reactor.h"
#include "detail/core/base/descriptor.h"

#if !defined(KNGIN_SYSTEM_WIN32)
#include <sys/eventfd.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

epoll_reactor::epoll_reactor ()
 : epoll_fd_(::epoll_create1(EPOLL_CLOEXEC)),
   waker_fd_(::eventfd(0, EFD_CLOEXEC)) {
  if (!epoll_fd_)
    throw_system_error("::epoll_create1() error", last_error());
  if (!waker_fd_) {
    throw_system_error("::eventfd(0, EFD_CLOEXEC)", last_error());
    ::close(epoll_fd_);
  }
}

epoll_reactor::~epoll_reactor () noexcept {
  TRY()
    this->close();
  IGNORE()
}

size_t
epoll_reactor::wait (op_queue &ops, timestamp delay) {

  ::epoll_event internal_events[KNGIN_EPOLL_REACTOR_MAX_EVENTS];
  auto size = ::epoll_wait(epoll_fd_, internal_events,
                           KNGIN_EPOLL_REACTOR_MAX_EVENTS, delay.value_int());
  auto ec = last_error();
  if (size < 0 and EINTR != ec)
    throw_system_error("::epoll_wait() error", last_error());
  for (int i = 0; i < size; ++i) {
    auto &internal_event = internal_events[i];
    auto *event = static_cast<class epoll_event *>(internal_event.data.ptr);
    assert(event);
    if (internal_event.events & (EPOLLERR | EPOLLHUP)) {
      ops.push(event->error_op_queue().top());
      continue;
    }
    if (internal_event.events & EPOLLIN)
      ops.push(event->read_op_queue().top());
    if (internal_event.events & EPOLLOUT)
      ops.push(event->write_op_queue().top());
  }
  return std::max<size_t>(size, 0);
}

void
epoll_reactor::wakeup () {
  ::epoll_event ev = {0, nullptr};
  if (::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, waker_fd_, &ev) < 0)
    throw_system_error("::epoll_ctl() error", last_error());
}

void
epoll_reactor::close () {
  if (epoll_fd_)
    descriptor::close(epoll_fd_);
  if (waker_fd_)
    descriptor::close(waker_fd_);
}

void
epoll_reactor::register_event (class epoll_event &ev) {
  assert(!ev.registed());
  ev.set_registed(true);
  update_event(EPOLL_CTL_ADD, ev.fd(), &ev);
}

void
epoll_reactor::remove_event (class epoll_event &ev) {
  assert(ev.registed());
  ev.set_registed(false);
  update_event(EPOLL_CTL_DEL, ev.fd(), &ev);
}

void
epoll_reactor::modify_event (class epoll_event &ev) {
  assert(ev.registed());
  update_event(EPOLL_CTL_MOD, ev.fd(), &ev);
}

void
epoll_reactor::update_event (int opt, int fd, class epoll_event *ev) {
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

  ::epoll_event internal_event = {ev->flags(), ev};
  if (::epoll_ctl(epoll_fd_, opt, fd, &internal_event) < 0)
    throw k::system_error("::epoll_ctl() error");
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#include "kngin/core/define.h"
#if !defined(KNGIN_SYSTEM_WIN32)

#include "kngin/core/base/system_error.h"
#include "detail/core/base/descriptor.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

size_t
descriptor::read (int fd, in_buffer &buf) {
  assert(buf.size() ? buf.writeable() : true);
  assert(FD_VALID(fd));
  auto size = ::read(fd, buf.begin(), buf.writeable());
  if (size < 0)
    throw_system_error("::read() error", last_error());
  buf += size;
  return size;
}

size_t
descriptor::read (int fd, in_buffer &buf, error_code &ec) noexcept {
  assert(buf.size() ? buf.writeable() : true);
  assert(FD_VALID(fd));
  auto size = ::read(fd, buf.begin(), buf.writeable());
  if (size < 0) {
    ec = last_error();
    return 0;
  } else {
    ec = error_code();
  }
  buf += size;
  return size;
}

size_t
descriptor::write (int fd, out_buffer &buf) {
  assert(buf.size() ? !buf.eof() : true);
  assert(FD_VALID(fd));
  auto size = ::write(fd, buf.begin(), buf.size());
  if (size < 0)
    throw_system_error("::write() error", last_error());
  buf -= size;
  return size;
}

size_t
descriptor::write (int fd, out_buffer &buf, error_code &ec) noexcept {
  assert(buf.size() ? !buf.eof() : true);
  assert(FD_VALID(fd));
  auto size = ::write(fd, buf.begin(), buf.size());
  if (size < 0) {
    ec = last_error();
    return 0;
  } else {
    ec = error_code();
  }
  buf -= size;
  return size;
}

size_t
descriptor::readn (int fd, in_buffer &buf) {
  assert(buf.size() ? buf.writeable() : true);
  assert(FD_VALID(fd));
  auto valid = buf.valid();
  while (buf.writeable()) {
    auto size = ::read(fd, buf.begin(), buf.writeable());
    if (size <= 0) {
      auto ec = last_error();
      if (EINTR == ec)
        continue;
      throw_system_error("::readn() error", ec);
    }
    buf += size;
  }
  return (buf.valid() - valid);
}

size_t
descriptor::readn (int fd, in_buffer &buf, error_code &ec) noexcept {
  assert(buf.size() ? buf.writeable() : true);
  assert(FD_VALID(fd));
  auto valid = buf.valid();
  while (buf.writeable()) {
    auto size = ::read(fd, buf.begin(), buf.writeable());
    if (size <= 0) {
      if (EINTR == (ec = last_error()) || EAGAIN == ec)
        continue;
      return (buf.valid() - valid);
    }
    buf += size;
  }
  ec = error_code();
  return (buf.valid() - valid);
}

size_t
descriptor::writen (int fd, out_buffer &buf) {
  assert(buf.size() ? !buf.eof() : true);
  assert(FD_VALID(fd));
  out_buffer buffer = std::move(buf);
  auto ret = buffer.size();
  while (buffer.size()) {
    auto size = ::write(fd, buffer.begin(), buffer.size());
    if (size < 0) {
      auto ec = last_error();
      if (EINTR == ec || EAGAIN == ec)
        continue;
      throw_system_error("::writen() error", ec);
    }
    buffer -= size;
  }
  return ret;
}

size_t
descriptor::writen (int fd, out_buffer &buf, error_code &ec) noexcept {
  assert(buf.size() ? !buf.eof() : true);
  assert(FD_VALID(fd));
  out_buffer buffer = std::move(buf);
  auto ret = buffer.size();
  while (buffer.size()) {
    auto size = ::write(fd, buffer.begin(), buffer.size());
    if (size < 0) {
      if (EINTR == (ec = last_error()))
        continue;
      return (ret - buffer.size());
    }
    buffer -= size;
  }
  ec = error_code();
  return ret;
}

size_t
descriptor::readable (int fd) {
  assert(FD_VALID(fd));
  size_t bytes = 0;
  if (::ioctl(fd, FIONREAD, &bytes) < 0)
    throw_system_error("::ioctl(FIONREAD) failed", last_error());
  return bytes;
}

size_t
descriptor::readable (int fd, error_code &ec) noexcept {
  assert(FD_VALID(fd));
  size_t bytes;
  ec = (::ioctl(fd, FIONREAD, &bytes) < 0) ? last_error() : error_code();
  return bytes;
}

error_code
descriptor::read_error (int fd) noexcept {
  assert(FD_VALID(fd));
  auto size = ::read(fd, nullptr, 0);
  if (size < 0)
    return last_error();
  return error_code();
}

void
descriptor::close (int &fd) {
  assert(FD_VALID(fd));
  if (::close(fd) < 0)
    throw_system_error("::close() error", last_error());
  fd = INVALID_FD;
}

void
descriptor::close (int &fd, error_code &ec) noexcept {
  assert(FD_VALID(fd));
  ec = (::close(fd) < 0) ? last_error() : error_code();
  fd = INVALID_FD;
}

int
descriptor::dup (int fd) {
  assert(FD_VALID(fd));
  auto new_fd = ::dup(fd);
  if (new_fd < 0)
    throw_system_error("::dup() error", last_error());
  return new_fd;
}

int
descriptor::dup (int fd, error_code &ec) noexcept {
  assert(FD_VALID(fd));
  auto new_fd = ::dup(fd);
  if (new_fd < 0) {
    ec = last_error();
    return INVALID_FD;
  }
  ec = error_code();
  return new_fd;
}

void
descriptor::set_nonblock (int fd, bool on) {
  assert(FD_VALID(fd));
  auto flags = ::fcntl(fd, F_GETFL, 0);
  flags = on ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
  if (::fcntl(fd, F_SETFL, flags) < 0)
    throw_system_error("::fcntl() set O_NONBLOCK flag failed", last_error());
}

void
descriptor::set_nonblock (int fd, bool on, error_code &ec) noexcept {
  assert(FD_VALID(fd));
  auto flags = ::fcntl(fd, F_GETFL, 0);
  flags = on ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
  ec = (::fcntl(fd, F_SETFL, flags) < 0) ? last_error() : error_code();
}

void
descriptor::set_closeexec (int fd, bool on) {
  assert(FD_VALID(fd));
  auto flags = ::fcntl(fd, F_GETFL, 0);
  flags = on ? flags | O_CLOEXEC : flags & ~O_CLOEXEC;
  if (::fcntl(fd, F_SETFL, flags) < 0)
    throw_system_error("::fcntl() set O_CLOEXEC flag failed", last_error());
}

void
descriptor::set_closeexec (int fd, bool on, error_code &ec) noexcept {
  assert(FD_VALID(fd));
  auto flags = ::fcntl(fd, F_GETFL, 0);
  flags = on ? flags | O_CLOEXEC : flags & ~O_CLOEXEC;
  ec = (::fcntl(fd, F_SETFL, flags) < 0) ? last_error() : error_code();
}

bool
descriptor::nonblock (int fd) {
  assert(FD_VALID(fd));
  auto flags = ::fcntl(fd, F_GETFL, 0);
  if (flags < 0)
    throw_system_error("::fcntl() get O_CLOEXEC flag failed", last_error());
  return (flags & O_NONBLOCK);
}

bool
descriptor::nonblock (int fd, error_code &ec) noexcept {
  assert(FD_VALID(fd));
  auto flags = ::fcntl(fd, F_GETFL, 0);
  ec = (flags < 0) ? last_error() : error_code();
  return (flags & O_NONBLOCK);
}

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_FILEFD

#ifdef _WIN32
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif
#include "core/base/system_error.h"
#include "core/base/common.h"
#include "core/base/filefd.h"

#define KNGIN_FILENAME "libkngin/core/base/filefd.cpp"

KNGIN_NAMESPACE_K_BEGIN

filefd_type filefd::invalid_fd = INVALID_FD;

filefd::filefd (filefd_type fd) noexcept
    : fd_(fd)
{
}

filefd::filefd (filefd &&fd) noexcept
    : fd_(fd.fd_)
{
    fd.fd_ = filefd::invalid_fd;
}

filefd::~filefd() noexcept
{
}

size_t
filefd::write (out_buffer &buf)
{
    assert(buf.size());
    assert(valid());
    auto size = ::write(fd_, buf.begin(), buf.size());
    if (size < 0)
        throw k::system_error("::write() error");
    buf -= size;
    return size;
}

size_t
filefd::write (out_buffer &buf, error_code &ec) noexcept
{
    assert(buf.size());
    assert(valid());
    auto size = ::write(fd_, buf.begin(), buf.size());
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
filefd::read (in_buffer &buf)
{
    assert(buf.writeable());
    assert(valid());
    auto size = ::read(fd_, buf.begin(), buf.writeable());
    if (size < 0)
        throw k::system_error("::read() error");
    buf += size;
    return size;
}

size_t
filefd::read (in_buffer &buf, error_code &ec) noexcept
{
    assert(buf.writeable());
    assert(valid());
    auto size = ::read(fd_, buf.begin(), buf.writeable());
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
filefd::writen (out_buffer &&buf)
{
    assert(buf.size());
    assert(valid());
    assert(!nonblock());
    out_buffer buffer = std::move(buf);
    auto ret = buffer.size();
    while (buffer.size()) {
        auto size = ::write(fd_, buffer.begin(), buffer.size());
        if (size < 0) {
            auto ec = last_error();
            if (EINTR == ec)
                continue;
            throw k::system_error("::writen() error", ec);
        }
        buffer -= size;
    }
    return ret;
}

size_t
filefd::writen (out_buffer &&buf, error_code &ec) noexcept
{
    assert(buf.size());
    assert(valid());
    assert(!nonblock());
    out_buffer buffer = std::move(buf);
    auto ret = buffer.size();
    while (buffer.size()) {
        auto size = ::write(fd_, buffer.begin(), buffer.size());
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
filefd::readn (in_buffer &buf)
{
    assert(buf.writeable());
    assert(valid());
    assert(!nonblock());
    while (buf.writeable()) {
        auto size = ::read(fd_, buf.begin(), buf.writeable());
        if (size < 0) {
            auto ec = last_error();
            if (EINTR == ec)
                continue;
            throw k::system_error("::readn() error", ec);
        }
        buf += size;
    }
    return buf.size();
}

size_t
filefd::readn (in_buffer &buf, error_code &ec) noexcept
{
    assert(buf.writeable());
    assert(valid());
    assert(!nonblock());
    while (buf.writeable()) {
        auto size = ::read(fd_, buf.begin(), buf.writeable());
        if (size < 0) {
            if (EINTR == (ec = last_error()))
                continue;
            return buf.valid();
        }
        buf += size;
    }
    ec = error_code();
    return buf.size();
}

//size_t
//filefd::writev (net_buffer &buf, size_t nbytes)
//{
//    assert(valid());
//    assert(buf.readable() >= nbytes);
//    auto size = ::writev(fd_, buf.to_iovec().data(), nbytes);
//    if (size < 0)
//        throw k::system_error("::writev() error");
//    buf.send(size);
//    return size;
//}
//
//size_t
//filefd::writev (net_buffer &buf, size_t nbytes, error_code &ec) noexcept
//{
//    assert(valid());
//    assert(buf.readable() >= nbytes);
//    auto size = ::writev(fd_, buf.to_iovec().data(), nbytes);
//    if (size < 0) {
//        ec = last_error();
//        return 0;
//    } else {
//        ec = error_code();
//    }
//    buf.send(size);
//    return size;
//}
//
//size_t
//filefd::readv (net_buffer &buf, size_t nbytes)
//{
//    assert(valid());
//    assert(buf.writeable() >= nbytes);
//    auto size = ::readv(fd_, buf.to_iovec().data(), nbytes);
//    if (size < 0)
//        throw k::system_error("::readv() error");
//    buf.receive(size);
//    return size;
//}

size_t
filefd::readable ()
{
    assert(valid());
    size_t bytes = 0;
    if (::ioctl(fd_, FIONREAD, &bytes) < 0)
        throw k::system_error("::ioctl(FIONREAD) failed");
    return bytes;
}

size_t
filefd::readable (error_code &ec) noexcept
{
    assert(valid());
    size_t bytes;
    ec = (::ioctl(fd_, FIONREAD, &bytes) < 0) ? last_error() : error_code();
    return bytes;
}

void
filefd::close ()
{
    if (invalid())
        return;
    if (::close(fd_) < 0) {
        fd_ = filefd::invalid_fd;
        throw k::system_error("::close() error");
    }
    fd_ = filefd::invalid_fd;
}

void
filefd::close (error_code &ec) noexcept
{
    if (invalid())
        return;
    ec = (::close(fd_) < 0) ? last_error() : error_code();
    fd_ = filefd::invalid_fd;
}

filefd_type
filefd::dup ()
{
    filefd_type new_fd = ::dup(fd_);
    if (new_fd < 0)
        throw k::system_error("::dup() error");
    return new_fd;
}

filefd_type
filefd::dup (error_code &ec)
{
    filefd_type new_fd = ::dup(fd_);
    if (new_fd < 0) {
        ec = last_error();
        return filefd::invalid_fd;
    }
    ec = error_code();
    return new_fd;
}

error_code
filefd::read_error () noexcept
{
    assert(valid());
    auto size = ::read(fd_, nullptr, 0);
    if (size < 0)
        return last_error();
    return error_code();
}

void
filefd::set_nonblock (bool on)
{
    assert(valid());
    filefd_type flags = ::fcntl(fd_, F_GETFL, 0);
    flags = on ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
    if (::fcntl(fd_, F_SETFL, flags) < 0)
        throw k::system_error("::fcntl() set O_NONBLOCK flag failed");
}

void
filefd::set_nonblock (bool on, error_code &ec) noexcept
{
    assert(valid());
    filefd_type flags = ::fcntl(fd_, F_GETFL, 0);
    flags = on ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
    ec = (::fcntl(fd_, F_SETFL, flags) < 0) ? last_error() : error_code();
}

void
filefd::set_closeexec (bool on)
{
    assert(valid());
    filefd_type flags = ::fcntl(fd_, F_GETFL, 0);
    flags = on ? flags | O_CLOEXEC : flags & ~O_CLOEXEC;
    if (::fcntl(fd_, F_SETFL, flags) < 0)
        throw k::system_error("::fcntl() set O_CLOEXEC flag failed");
}

void
filefd::set_closeexec (bool on, error_code &ec) noexcept
{
    assert(valid());
    filefd_type flags = ::fcntl(fd_, F_GETFL, 0);
    flags = on ? flags | O_CLOEXEC : flags & ~O_CLOEXEC;
    ec = (::fcntl(fd_, F_SETFL, flags) < 0) ? last_error() : error_code();
}

bool
filefd::nonblock () const
{
    assert(valid());
    filefd_type flags = ::fcntl(fd_, F_GETFL, 0);
    if (flags < 0)
        throw k::system_error("::fcntl() get O_CLOEXEC flag failed");
    return (flags & O_NONBLOCK);
}

bool
filefd::nonblock (error_code &ec) const noexcept
{
    assert(valid());
    filefd_type flags = ::fcntl(fd_, F_GETFL, 0);
    ec = (flags < 0) ? last_error() : error_code();
    return (flags & O_NONBLOCK);
}

filefd &
filefd::operator = (filefd_type fd) noexcept
{
    assert(FD_VALID(fd));
    fd_ = fd;
    return *this;
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_FILEFD */

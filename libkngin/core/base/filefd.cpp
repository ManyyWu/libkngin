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

int filefd::invalid_fd = INVALID_FD;

filefd::filefd (int _fd) KNGIN_NOEXCP
    : m_fd(_fd)
{
}

filefd::filefd (filefd &&_fd) KNGIN_NOEXCP
    : m_fd(_fd.m_fd)
{
    _fd.m_fd = filefd::invalid_fd;
}

filefd::~filefd() KNGIN_NOEXCP
{
}

size_t
filefd::write (out_buffer &_buf)
{
    assert(_buf.size());
    assert(valid());
    ssize_t _size = ::write(m_fd, _buf.begin(), _buf.size());
    if (_size < 0)
        throw k::system_error("::write() error");
    _buf -= _size;
    return _size;
}

size_t
filefd::write (out_buffer &_buf, error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.size());
    assert(valid());
    ssize_t _size = ::write(m_fd, _buf.begin(), _buf.size());
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = error_code();
    }
    _buf -= _size;
    return _size;
}

size_t
filefd::read (in_buffer &_buf)
{
    assert(_buf.writeable());
    assert(valid());
    ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
    if (_size < 0)
        throw k::system_error("::read() error");
    _buf += _size;
    return _size;
}

size_t
filefd::read (in_buffer &_buf, error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.writeable());
    assert(valid());
    ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = error_code();
    }
    _buf += _size;
    return _size;
}

size_t
filefd::writen (out_buffer &&_buf)
{
    assert(_buf.size());
    assert(valid());
    assert(!nonblock());
    out_buffer _buffer = std::move(_buf);
    size_t _ret = _buffer.size();
    while (_buffer.size()) {
        ssize_t _size = ::write(m_fd, _buffer.begin(), _buffer.size());
        if (_size < 0) {
            auto _ec = last_error();
            if (EINTR == _ec)
                continue;
            throw k::system_error("::writen() error", _ec);
        }
        _buffer -= _size;
    }
    return _ret;
}

size_t
filefd::writen (out_buffer &&_buf, error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.size());
    assert(valid());
    assert(!nonblock());
    out_buffer _buffer = std::move(_buf);
    size_t _ret = _buffer.size();
    while (_buffer.size()) {
        ssize_t _size = ::write(m_fd, _buffer.begin(), _buffer.size());
        if (_size < 0) {
            if (EINTR == (_ec = last_error()))
                continue;
            return (_ret - _buffer.size());
        }
        _buffer -= _size;
    }
    _ec = error_code();
    return _ret;
}

size_t
filefd::readn (in_buffer &_buf)
{
    assert(_buf.writeable());
    assert(valid());
    assert(!nonblock());
    while (_buf.writeable()) {
        ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
        if (_size < 0) {
            auto _ec = last_error();
            if (EINTR == _ec)
                continue;
            throw k::system_error("::readn() error", _ec);
        }
        _buf += _size;
    }
    return _buf.size();
}

size_t
filefd::readn (in_buffer &_buf, error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.writeable());
    assert(valid());
    assert(!nonblock());
    while (_buf.writeable()) {
        ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
        if (_size < 0) {
            if (EINTR == (_ec = last_error()))
                continue;
            return _buf.valid();
        }
        _buf += _size;
    }
    _ec = error_code();
    return _buf.size();
}

//size_t
//filefd::writev (net_buffer &_buf, size_t _nbytes)
//{
//    assert(valid());
//    assert(_buf.readable() >= _nbytes);
//    ssize_t _size = ::writev(m_fd, _buf.to_iovec().data(), _nbytes);
//    if (_size < 0)
//        throw k::system_error("::writev() error");
//    _buf.send(_size);
//    return _size;
//}
//
//size_t
//filefd::writev (net_buffer &_buf, size_t _nbytes, error_code &_ec) KNGIN_NOEXCP
//{
//    assert(valid());
//    assert(_buf.readable() >= _nbytes);
//    ssize_t _size = ::writev(m_fd, _buf.to_iovec().data(), _nbytes);
//    if (_size < 0) {
//        _ec = last_error();
//        return 0;
//    } else {
//        _ec = error_code();
//    }
//    _buf.send(_size);
//    return _size;
//}
//
//size_t
//filefd::readv (net_buffer &_buf, size_t _nbytes)
//{
//    assert(valid());
//    assert(_buf.writeable() >= _nbytes);
//    ssize_t _size = ::readv(m_fd, _buf.to_iovec().data(), _nbytes);
//    if (_size < 0)
//        throw k::system_error("::readv() error");
//    _buf.receive(_size);
//    return _size;
//}

size_t
filefd::readable ()
{
    assert(valid());
    size_t _bytes = 0;
    if (::ioctl(m_fd, FIONREAD, &_bytes) < 0)
        throw k::system_error("::ioctl(FIONREAD) failed");
    return _bytes;
}

size_t
filefd::readable (error_code &_ec) KNGIN_NOEXCP
{
    assert(valid());
    size_t _bytes;
    _ec = (::ioctl(m_fd, FIONREAD, &_bytes) < 0) ? last_error() : error_code();
    return _bytes;
}

void
filefd::close ()
{
    if (invalid())
        return;
    if (::close(m_fd) < 0) {
        m_fd = filefd::invalid_fd;
        throw k::system_error("::close() error");
    }
    m_fd = filefd::invalid_fd;
}

void
filefd::close (error_code &_ec) KNGIN_NOEXCP
{
    if (invalid())
        return;
    _ec = (::close(m_fd) < 0) ? last_error() : error_code();
    m_fd = filefd::invalid_fd;
}

int
filefd::dup ()
{
    int _new_fd = ::dup(m_fd);
    if (_new_fd < 0)
        throw k::system_error("::dup() error");
    return _new_fd;
}

int
filefd::dup (error_code &_ec)
{
    int _new_fd = ::dup(m_fd);
    if (_new_fd < 0) {
        _ec = last_error();
        return filefd::invalid_fd;
    }
    _ec = error_code();
    return _new_fd;
}

error_code
filefd::read_error () KNGIN_NOEXCP
{
    assert(valid());
    ssize_t _size = ::read(m_fd, nullptr, 0);
    if (_size < 0)
        return last_error();
    return error_code();
}

void
filefd::set_nonblock (bool _on)
{
    assert(valid());
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    if (::fcntl(m_fd, F_SETFL, _flags) < 0)
        throw k::system_error("::fcntl() set O_NONBLOCK flag failed");
}

void
filefd::set_nonblock (bool _on, error_code &_ec) KNGIN_NOEXCP
{
    assert(valid());
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    _ec = (::fcntl(m_fd, F_SETFL, _flags) < 0) ? last_error() : error_code();
}

void
filefd::set_closeexec (bool _on)
{
    assert(valid());
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    if (::fcntl(m_fd, F_SETFL, _flags) < 0)
        throw k::system_error("::fcntl() set O_CLOEXEC flag failed");
}

void
filefd::set_closeexec (bool _on, error_code &_ec) KNGIN_NOEXCP
{
    assert(valid());
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    _ec = (::fcntl(m_fd, F_SETFL, _flags) < 0) ? last_error() : error_code();
}

bool
filefd::nonblock () const
{
    assert(valid());
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    if (_flags < 0)
        throw k::system_error("::fcntl() get O_CLOEXEC flag failed");
    return (_flags & O_NONBLOCK);
}

bool
filefd::nonblock (error_code &_ec) const KNGIN_NOEXCP
{
    assert(valid());
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _ec = (_flags < 0) ? last_error() : error_code();
    return (_flags & O_NONBLOCK);
}

filefd &
filefd::operator = (int _fd) KNGIN_NOEXCP
{
    assert(FD_VALID(_fd));
    m_fd = _fd;
    return *this;
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_FILEFD */

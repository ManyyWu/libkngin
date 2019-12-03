#ifdef _WIN32
#else
#include <unistd.h>
#include <fcntl.h>
#endif
#include <cstring>
#include "core/system_error.h"
#include "core/common.h"
#include "net/filefd.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/filefd.cpp"

KNGIN_NAMESPACE_K_BEGIN

int filefd::invalid_fd = INVALID_FD;

filefd::filefd (int _fd) KNGIN_NOEXP
    : m_fd(_fd)
{
}

filefd::filefd (filefd &&_fd) KNGIN_NOEXP
    : m_fd(_fd.m_fd)
{
}

filefd::~filefd() KNGIN_NOEXP
{
    std::error_code _ec;
    close(_ec);
    if (_ec)
        log_error("::close() error, fd = %d, %s:%d",
                  m_fd, _ec.message().c_str(), _ec.value());
}

size_t
filefd::write (out_buffer &_buf)
{
    arg_check(_buf.size());
    check(FD_VALID(m_fd));
    ssize_t _size = ::write(m_fd, _buf.begin(), _buf.size());
    if (_size < 0)
        throw k::system_error("::write() error");
    _buf -= _size;
    return _size;
}

size_t
filefd::write (out_buffer &_buf, std::error_code &_ec) KNGIN_NOEXP
{
    arg_check(_buf.size());
    check(FD_VALID(m_fd));
    ssize_t _size = ::write(m_fd, _buf.begin(), _buf.size());
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf -= _size;
    return _size;
}

size_t
filefd::read (in_buffer &_buf)
{
    arg_check(_buf.writeable());
    check(FD_VALID(m_fd));
    ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
    if (_size < 0)
        throw k::system_error("::read() error");
    _buf += _size;
    return _size;
}

size_t
filefd::read (in_buffer &_buf, std::error_code &_ec) KNGIN_NOEXP
{
    arg_check(_buf.writeable());
    check(FD_VALID(m_fd));
    ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf += _size;
    return _size;
}

size_t
filefd::writen (out_buffer &&_buf)
{
    arg_check(_buf.size());
    check(FD_VALID(m_fd));
    assert(!this->nonblock());
    out_buffer _buffer = std::move(_buf);
    size_t _ret = _buffer.size();
    while (_buffer.size()) {
        ssize_t _size = ::write(m_fd, _buffer.begin(), _buffer.size());
        if (_size < 0) {
            std::error_code _ec = last_error();
            if (_ec == std::errc::interrupted)
                continue;
            throw k::system_error("::writen() error", _ec);
        }
        _buffer -= _size;
    }
    return _ret;
}

size_t
filefd::writen (out_buffer &&_buf, std::error_code &_ec) KNGIN_NOEXP
{
    arg_check(_buf.size());
    check(FD_VALID(m_fd));
    assert(!this->nonblock());
    out_buffer _buffer = std::move(_buf);
    size_t _ret = _buffer.size();
    while (_buffer.size()) {
        ssize_t _size = ::write(m_fd, _buffer.begin(), _buffer.size());
        if (_size < 0) {
            if ((_ec = last_error()) == std::errc::interrupted)
                continue;
            return 0;
        }
        _buffer -= _size;
    }
    _ec = std::error_code();
    return _ret;
}

size_t
filefd::readn (in_buffer &_buf)
{
    arg_check(_buf.writeable());
    check(FD_VALID(m_fd));
    assert(!this->nonblock());
    while (_buf.writeable()) {
        ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
        if (_size < 0) {
            std::error_code _ec = last_error();
            if (_ec == std::errc::interrupted)
                continue;
            throw k::system_error("::readn() error", _ec);
        }
        _buf += _size;
    }
    return _buf.size();
}

size_t
filefd::readn (in_buffer &_buf, std::error_code &_ec) KNGIN_NOEXP
{
    arg_check(_buf.writeable());
    check(FD_VALID(m_fd));
    assert(!this->nonblock());
    while (_buf.writeable()) {
        ssize_t _size = ::read(m_fd, _buf.begin(), _buf.writeable());
        if (_size < 0) {
            if ((_ec = last_error()) == std::errc::interrupted)
                continue;
            return 0;
        }
        _buf += _size;
    }
    _ec = std::error_code();
    return _buf.size();
}

//size_t
//filefd::writev (net_buffer &_buf, size_t _nbytes)
//{
//    assert(FD_VALID(m_fd));
//    assert(_buf.readable() >= _nbytes);
//    ssize_t _size = ::writev(m_fd, _buf.to_iovec().data(), _nbytes);
//    if (_size < 0)
//        throw k::system_error("::writev() error");
//    _buf.send(_size);
//    return _size;
//}
//
//size_t
//filefd::writev (net_buffer &_buf, size_t _nbytes, std::error_code &_ec) KNGIN_NOEXP
//{
//    assert(FD_VALID(m_fd));
//    assert(_buf.readable() >= _nbytes);
//    ssize_t _size = ::writev(m_fd, _buf.to_iovec().data(), _nbytes);
//    if (_size < 0) {
//        _ec = last_error();
//        return 0;
//    } else {
//        _ec = std::error_code();
//    }
//    _buf.send(_size);
//    return _size;
//}
//
//size_t
//filefd::readv (net_buffer &_buf, size_t _nbytes)
//{
//    assert(FD_VALID(m_fd));
//    assert(_buf.writeable() >= _nbytes);
//    ssize_t _size = ::readv(m_fd, _buf.to_iovec().data(), _nbytes);
//    if (_size < 0)
//        throw k::system_error("::readv() error");
//    _buf.receive(_size);
//    return _size;
//}

void
filefd::close ()
{
    if (FD_INVALID(m_fd))
        return;
    if (::close(m_fd) < 0) {
        m_fd = filefd::invalid_fd;
        throw k::system_error("::close() error");
    }
    m_fd = filefd::invalid_fd;
}

void
filefd::close (std::error_code &_ec) KNGIN_NOEXP
{
    if (FD_INVALID(m_fd))
        return;
    _ec = (::close(m_fd) < 0) ? last_error() : std::error_code();
    m_fd = filefd::invalid_fd;
}

std::error_code
filefd::read_error () KNGIN_NOEXP
{
    check(FD_VALID(m_fd));
    ssize_t _size = ::read(m_fd, NULL, 0);
    if (_size < 0)
        return last_error();
    return std::error_code();
}

void
filefd::set_nonblock (bool _on)
{
    check(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    if (::fcntl(m_fd, F_SETFL, _flags) < 0)
        throw k::system_error("::fcntl() set O_NONBLOCK flag failed");
}

void
filefd::set_nonblock (bool _on, std::error_code &_ec) KNGIN_NOEXP
{
    check(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    _ec = (::fcntl(m_fd, F_SETFL, _flags) < 0) ? last_error() : std::error_code();
}

void
filefd::set_closeexec (bool _on)
{
    check(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    if (::fcntl(m_fd, F_SETFL, _flags) < 0)
        throw k::system_error("::fcntl() set O_CLOEXEC flag failed");
}

void
filefd::set_closeexec (bool _on, std::error_code &_ec) KNGIN_NOEXP
{
    check(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    _ec = (::fcntl(m_fd, F_SETFL, _flags) < 0) ? last_error() : std::error_code();
}

bool
filefd::nonblock () const
{
    check(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    if (_flags < 0)
        throw k::system_error("::fcntl() get O_CLOEXEC flag failed");
    return (_flags & O_NONBLOCK);
}

bool
filefd::nonblock (std::error_code &_ec) const KNGIN_NOEXP
{
    check(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _ec = (_flags < 0) ? last_error() : std::error_code();
    return (_flags & O_NONBLOCK);
}

filefd &
filefd::operator = (int _fd) KNGIN_NOEXP
{
    check(FD_VALID(m_fd));
    m_fd = _fd;
    return *this;
}

KNGIN_NAMESPACE_K_END

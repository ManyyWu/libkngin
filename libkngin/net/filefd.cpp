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
filefd::write (buffer &_buf, size_t _nbytes) KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    assert(_buf.readable(_nbytes));
    ssize_t _size = ::write(m_fd, _buf.data() + _buf.rindex(), _nbytes);
    if (_size < 0)
        throw k::system_error("::write() error");
    _buf.rreset(_buf.rindex() + _size);
    return _size;
}

size_t
filefd::write (buffer &_buf, size_t _nbytes, std::error_code &_ec) KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    assert(_buf.readable(_nbytes));
    ssize_t _size = ::write(m_fd, _buf.data() + _buf.rindex(), _nbytes);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf.rreset(_buf.rindex() + _size);
    return _size;
}

size_t
filefd::read (buffer &_buf, size_t _nbytes) KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    assert(_buf.writeable(_nbytes));
    ssize_t _size = ::read(m_fd, _buf.data() + _buf.windex(), _nbytes);
    if (_size < 0)
        throw k::system_error("::read() error");
    _buf.wreset(_buf.windex() + _size);
    return _size;
}

size_t
filefd::read (buffer &_buf, size_t _nbytes, std::error_code &_ec) KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    assert(_buf.writeable(_nbytes));
    ssize_t _size = ::read(m_fd, _buf.data() + _buf.windex(), _nbytes);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf.wreset(_buf.windex() + _size);
    return _size;
}

size_t
filefd::writev (net_buffer &_buf, size_t _nbytes) KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    assert(_buf.readable() >= _nbytes);
    ssize_t _size = ::writev(m_fd, _buf.to_iovec().data(), _nbytes);
    if (_size < 0)
        throw k::system_error("::writev() error");
    _buf.send(_size);
    return _size;
}

size_t
filefd::writev (net_buffer &_buf, size_t _nbytes, std::error_code &_ec) KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    assert(_buf.readable() >= _nbytes);
    ssize_t _size = ::writev(m_fd, _buf.to_iovec().data(), _nbytes);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf.send(_size);
    return _size;
}

size_t
filefd::readv (net_buffer &_buf, size_t _nbytes) KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    assert(_buf.writeable() >= _nbytes);
    ssize_t _size = ::readv(m_fd, _buf.to_iovec().data(), _nbytes);
    if (_size < 0)
        throw k::system_error("::readv() error");
    _buf.receive(_size);
    return _size;
}

void
filefd::close () KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    assert(FD_VALID(m_fd));
    if (::close(m_fd) < 0)
        throw k::system_error("::close() error");
    m_fd = filefd::invalid_fd;
}

void
filefd::close (std::error_code &_ec) KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    assert(FD_VALID(m_fd));
    _ec = (::close(m_fd) < 0) ? last_error() : std::error_code();
    m_fd = filefd::invalid_fd;
}

void
filefd::set_nonblock (bool _on) KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    if (::fcntl(m_fd, F_SETFL, _flags) < 0)
        throw k::system_error("::fcntl() set O_NONBLOCK flag failed");
}

void
filefd::set_nonblock (bool _on, std::error_code &_ec) KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    _ec = (::fcntl(m_fd, F_SETFL, _flags) < 0) ? last_error() : std::error_code();
}

void
filefd::set_closeexec (bool _on) KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    if (::fcntl(m_fd, F_SETFL, _flags) < 0)
        throw k::system_error("::fcntl() set O_CLOEXEC flag failed");
}

void
filefd::set_closeexec (bool _on, std::error_code &_ec) KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    _ec = (::fcntl(m_fd, F_SETFL, _flags) < 0) ? last_error() : std::error_code();
}

bool
filefd::nonblock () const KNGIN_EXP
{
    assert(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    if (_flags < 0)
        throw k::system_error("::fcntl() get O_CLOEXEC flag failed");
    return (_flags & O_NONBLOCK);
}

bool
filefd::nonblock (std::error_code &_ec) const KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _ec = (_flags & O_NONBLOCK) ? last_error() : std::error_code();
    return (_flags & O_NONBLOCK);
}

filefd &
filefd::operator = (int _fd) KNGIN_NOEXP
{
    assert(FD_VALID(m_fd));
    m_fd = _fd;
    return *this;
}

KNGIN_NAMESPACE_K_END

#ifdef _WIN32
#else
#include <unistd.h>
#include <fcntl.h>
#endif
#include <cstring>
#include "filefd.h"
#include "common.h"
#include "buffer.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/filefd.cpp"

KNGIN_NAMESPACE_K_BEGIN

int filefd::invalid_fd = INVALID_FD;

filefd::filefd (int _fd)
    : m_fd(_fd)
{
    if (fd_invalid(_fd))
        m_fd = filefd::invalid_fd;
}

filefd::filefd (filefd &&_fd)
    : m_fd(_fd.m_fd)
{
    _fd.m_fd = filefd::invalid_fd;
}

filefd::~filefd()
{
    if (fd_valid(m_fd))
        this->close();
}

ssize_t
filefd::write (buffer &_buf, size_t _nbytes)
{
    check(_buf.readable(_nbytes));
    ssize_t _size = ::write(m_fd, _buf.data() + _buf.rindex(), _nbytes);
    if (_size > 0)
        _buf.rreset(_buf.rindex() + _size);
    return _size;
};

ssize_t
filefd::read (buffer &_buf, size_t _nbytes)
{
    check(_buf.writeable(_nbytes));
    ssize_t _size = ::read(m_fd, _buf.data() + _buf.windex(), _nbytes);
    if (_size > 0)
        _buf.wreset(_buf.windex() + _size);
    return _size;
}

ssize_t
filefd::writev (net_buffer &_buf, size_t _nbytes)
{
    check(_buf.readable() >= _nbytes);
    ssize_t _size = ::writev(m_fd, _buf.to_iovec().data(), _nbytes);
    if (_size > 0)
        _buf.send(_size);
    return _size;
}

ssize_t
filefd::readv (net_buffer &_buf, size_t _nbytes)
{
    check(_buf.writeable() >= _nbytes);
    ssize_t _size = ::readv(m_fd, _buf.to_iovec().data(), _nbytes);
    if (_size > 0)
        _buf.receive(_size);
    return _size;
}

void
filefd::close ()
{
    check(fd_valid(m_fd));
    int _ret = ::close(m_fd);
    if (_ret < 0)
        log_error("::close() error - %s:%d", strerror(errno), errno);
    m_fd = filefd::invalid_fd;
}

bool
filefd::set_nonblock (bool _on /* = true */)
{
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    int _ret = ::fcntl(m_fd, F_SETFL, _flags);
    if (_ret < 0) 
        log_error("::fcntl() set O_NONBLOCK flag failed - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
 } 

bool
filefd::set_closeexec (bool _on /* = true */)
{
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    int _ret = ::fcntl(m_fd, F_SETFL, _flags);
    if (_ret < 0) 
        log_error("::fcntl() set O_CLOEXEC flag failed - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
}

bool
filefd::nonblock () const
{
    int _flags = ::fcntl(m_fd, F_GETFL, 0);
    if (_flags < 0)
        log_error("::fcntl() set O_CLOEXEC flag failed - %s:%d", strerror(errno), errno);
    return _flags & O_NONBLOCK;
}

bool
filefd::reuse_addr () const
{
    return ::fcntl(m_fd, F_GETFL, 0) & O_CLOEXEC;
}

filefd &
filefd::operator = (int _fd)
{
    check(fd_valid(_fd));
    m_fd = _fd;
    return *this;
}

KNGIN_NAMESPACE_K_END

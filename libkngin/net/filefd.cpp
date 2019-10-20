#ifdef _WIN32
#include <Windows.h>
#
#else
#include <unistd.h>
#include <fcntl.h>
#endif
#include <cstring>
#include "define.h"
#include "error.h"
#include "filefd.h"
#include "common.h"
#include "buffer.h"

__NAMESPACE_BEGIN

filefd::filefd (int _fd)
    : m_fd(_fd)
{
    // kassert(__fd_valid(_fd));
}

filefd::~filefd()
{
    if (__fd_valid(m_fd))
        ::close(m_fd);
}

int
filefd::fd () const
{
    return m_fd;
}

ssize_t
filefd::write (const buffer &_buf, size_t _nbytes)
{
    kassert(_buf.size() < _nbytes);
    return ::write(m_fd, _buf.get().data(), _nbytes);
};

ssize_t
filefd::read (buffer &_buf, size_t _nbytes)
{
    kassert(_buf.size() < _nbytes);
    return ::read(m_fd, _buf.get().data(), _nbytes);
}

void
filefd::close ()
{
    kassert(__fd_valid(m_fd));
    int _ret = ::close(m_fd);
    if (_ret < 0)
        log_error("::close() error - %s:%d", strerror(errno), errno);
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

__NAMESPACE_END

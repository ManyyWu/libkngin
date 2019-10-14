#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "define.h"
#include "filefd.h"
#include "common.h"

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
filefd::write (void *_buf, size_t _nbytes)
{
    kassert(__fd_valid(m_fd));
    return ::write(m_fd, _buf, _nbytes);
}

ssize_t
filefd::read (void *_buf, size_t _nbytes)
{
    kassert(__fd_valid(m_fd));
    return ::read(m_fd, _buf, _nbytes);
}

void
filefd::close ()
{
    kassert(__fd_valid(m_fd));
    ::close(m_fd);
}

bool
filefd::set_nonblock (bool _on /* = true */)
{
    int _flags = ::fcntl(m_fd, F_GETFL, _flags)
    _flags = _on ? _flags | O_NONBLOCK : _flags & ~O_NONBLOCK;
    int _ret = ::fcntl(m_fd, F_SETFL, _flags);
    if (_ret < 0) 
        log_error("::fcntl() set O_NONBLOCK flag failed - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
 } 

bool
filefd::set_closeexec (bool _on /* = true */)
{
    int _flags = ::fcntl(m_fd, F_GETFL, _flags)
    _flags = _on ? _flags | O_CLOEXEC : _flags & ~O_CLOEXEC;
    int _ret = ::fcntl(m_fd, F_SETFL, _flags);
    if (_ret < 0) 
        log_error("::fcntl() set O_CLOEXEC flag failed - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
}

bool
filefd::nonblock () const
{
    return ::fcntl(m_fd, F_GETFL, _flags) & O_NONBLOCK;
}

bool
filefd::reuse_addr () const
{
    return ::fcntl(m_fd, F_GETFL, _flags) & O_CLOEXEC;
}

__NAMESPACE_END

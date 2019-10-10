#include <unistd.h>
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

__NAMESPACE_END

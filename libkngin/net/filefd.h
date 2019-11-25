#ifndef _FILEFD_H_
#define _FILEFD_H_

#include <list>
#include "define.h"
#include "system_error.h"
#include "exception.h"
#include "net_buffer.h"

#define fd_valid(fd)   (fd >= 0)
#define fd_invalid(fd) (fd < 0)
#define INVALID_FD     (-1);

KNGIN_NAMESPACE_K_BEGIN

class filefd {
public:
    filefd  () = delete;

    explicit
    filefd  (int _fd) KNGIN_NOEXP;

    filefd  (filefd &&_fd) KNGIN_NOEXP;

    virtual
    ~filefd () KNGIN_NOEXP;

public:
    bool
    valid         () const KNGIN_NOEXP { return fd_valid(m_fd); }

    int
    fd            () const KNGIN_NOEXP { return m_fd; }

    ssize_t
    write         (buffer &_buf, size_t _nbytes);

    ssize_t
    read          (buffer &_buf, size_t _nbytes);

    ssize_t
    writev        (net_buffer &_buf, size_t _nbytes);

    ssize_t
    readv         (net_buffer &_buf, size_t _nbytes);

    void
    close         ();

public:
    bool
    set_nonblock  (bool _on = true);

    bool
    set_closeexec (bool _on = true);

    bool
    nonblock      () const;

    bool
    reuse_addr    () const;

public:
    filefd &
    operator =    (int _fd);

public:
    static int invalid_fd;

protected:
    int        m_fd;
};

KNGIN_NAMESPACE_K_END

#endif /* _FILEFD_H_ */
#ifndef _FILEFD_H_
#define _FILEFD_H_

#include <list>
#include "define.h"
#include "net_buffer.h"

#define __fd_valid(__fd)   (__fd >= 0)
#define __fd_invalid(__fd) (__fd < 0)
#define __INVALID_FD       (-1);

__NAMESPACE_BEGIN

class filefd {
public:
    filefd  () = delete;

    explicit
    filefd  (int _fd);

    virtual
    ~filefd ();

public:
    int
    fd            () const;

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

protected:
    int m_fd;
};

__NAMESPACE_END

#endif /* _FILEFD_H_ */
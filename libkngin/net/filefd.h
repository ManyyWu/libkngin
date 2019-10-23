#ifndef _FILEFD_H_
#define _FILEFD_H_

#include "define.h"
#include "noncopyable.h"
#include "buffer.h"

#define __fd_valid(__fd)   (__fd >= 0)
#define __fd_invalid(__fd) (__fd < 0)

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
    write         (const buffer &_buf, size_t _nbytes);

    ssize_t
    read          (buffer &_buf, size_t _nbytes);

    ssize_t
    writev        (const std::vector<buffer> &_buf, size_t _n);

    ssize_t
    readv         (std::vector<buffer> &_buf, size_t _n);

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
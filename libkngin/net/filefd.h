#ifndef _FILEFD_H_
#define _FILEFD_H_

#include <list>
#include <system_error>
#include "core/define.h"
#include "core/exception.h"
#include "net/net_buffer.h"

#define FD_VALID(fd)   (fd >= 0)
#define FD_INVALID(fd) (fd < 0)
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
    valid         () const KNGIN_NOEXP { return FD_VALID(m_fd); }

    int
    fd            () const KNGIN_NOEXP { return m_fd; }

    size_t
    write         (buffer &_buf, size_t _nbytes) KNGIN_EXP;

    size_t
    write         (buffer &_buf, size_t _nbytes, std::error_code &_ec) KNGIN_NOEXP;

    size_t
    read          (buffer &_buf, size_t _nbytes) KNGIN_EXP;

    size_t
    read          (buffer &_buf, size_t _nbytes, std::error_code &_ec) KNGIN_NOEXP;

    size_t
    writev        (net_buffer &_buf, size_t _nbytes) KNGIN_EXP;

    size_t
    writev        (net_buffer &_buf, size_t _nbytes, std::error_code &_ec) KNGIN_NOEXP;

    size_t
    readv         (net_buffer &_buf, size_t _nbytes) KNGIN_EXP;

    void
    close         () KNGIN_EXP;

    void
    close         (std::error_code &_ec) KNGIN_NOEXP;

public:
    void
    set_nonblock  (bool _on) KNGIN_EXP;

    void
    set_nonblock  (bool _on, std::error_code &_ec) KNGIN_NOEXP;

    void
    set_closeexec (bool _on) KNGIN_EXP;

    void
    set_closeexec (bool _on, std::error_code &_ec) KNGIN_NOEXP;

    bool
    nonblock      () const KNGIN_EXP;

    bool
    nonblock      (std::error_code &_ec) const KNGIN_NOEXP;

public:
    filefd &
    operator =    (int _fd) KNGIN_NOEXP;

public:
    static int invalid_fd;

protected:
    int        m_fd;
};

KNGIN_NAMESPACE_K_END

#endif /* _FILEFD_H_ */
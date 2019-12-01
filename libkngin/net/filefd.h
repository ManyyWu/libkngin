#ifndef _FILEFD_H_
#define _FILEFD_H_

#include <list>
#include <system_error>
#include "core/define.h"
#include "core/noncopyable.h"
#include "core/exception.h"
#include "core/buffer.h"

#define FD_VALID(fd)   (fd >= 0)
#define FD_INVALID(fd) (fd < 0)
#define INVALID_FD     (-1);

KNGIN_NAMESPACE_K_BEGIN

class filefd : public noncopyable {
public:
    filefd  () = delete;

    explicit
    filefd  (int _fd) KNGIN_NOEXP;

    filefd  (filefd &&_fd) KNGIN_NOEXP;

    virtual
    ~filefd () KNGIN_NOEXP;

public:
    bool
    valid         () const KNGIN_NOEXP
    { return FD_VALID(m_fd); }

    int
    fd            () const KNGIN_NOEXP
    { return m_fd; }

    size_t
    write         (out_buffer &_buf);

    size_t
    write         (out_buffer &_buf, std::error_code &_ec) KNGIN_NOEXP;

    size_t
    read          (in_buffer &_buf);

    size_t
    read          (in_buffer &_buf, std::error_code &_ec) KNGIN_NOEXP;

    size_t
    writen        (out_buffer &&_buf);

    size_t
    writen        (out_buffer &&_buf, std::error_code &_ec) KNGIN_NOEXP;

    size_t
    readn         (in_buffer &_buf);

    size_t
    readn         (in_buffer &_buf, std::error_code &_ec) KNGIN_NOEXP;

//    size_t
//    writev        (out_vector &_buf);
//
//    size_t
//    writev        (out_vector &_buf, std::error_code &_ec) KNGIN_NOEXP;
//
//    size_t
//    readv         (in_vector &_buf);
//
//    size_t
//    readv         (in_vector &_buf, std::error_code &_ec);

    void
    close         ();

    void
    close         (std::error_code &_ec) KNGIN_NOEXP;

public:
    void
    set_nonblock  (bool _on);

    void
    set_nonblock  (bool _on, std::error_code &_ec) KNGIN_NOEXP;

    void
    set_closeexec (bool _on);

    void
    set_closeexec (bool _on, std::error_code &_ec) KNGIN_NOEXP;

    bool
    nonblock      () const;

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
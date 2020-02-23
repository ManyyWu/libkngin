#ifndef KNGIN_FILEFD_H
#define KNGIN_FILEFD_H

#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_FILEFD

#include <list>
#include "core/base/noncopyable.h"
#include "core/base/exception.h"
#include "core/base/error_code.h"
#include "core/base/buffer.h"

KNGIN_NAMESPACE_K_BEGIN

typedef filefd_type int;

#define FD_VALID(fd)   (filefd_type(fd) >= 0)
#define FD_INVALID(fd) (filefd_type(fd) < 0)
#define INVALID_FD     (-1);

class filefd : public noncopyable {
public:
    filefd        () = delete;

    explicit
    filefd        (filefd_type _fd) KNGIN_NOEXCP;

    filefd        (filefd &&_fd) KNGIN_NOEXCP;

    virtual
    ~filefd       () KNGIN_NOEXCP;

public:
    bool
    valid         () const KNGIN_NOEXCP
    { return FD_VALID(m_fd); }

    bool
    invalid       () const KNGIN_NOEXCP
    { return FD_INVALID(m_fd); }

    filefd_type
    fd            () const KNGIN_NOEXCP
    { return m_fd; }

    size_t
    write         (out_buffer &_buf);

    size_t
    write         (out_buffer &_buf, error_code &_ec) KNGIN_NOEXCP;

    size_t
    read          (in_buffer &_buf);

    size_t
    read          (in_buffer &_buf, error_code &_ec) KNGIN_NOEXCP;

    size_t
    writen        (out_buffer &&_buf);

    size_t
    writen        (out_buffer &&_buf, error_code &_ec) KNGIN_NOEXCP;

    size_t
    readn         (in_buffer &_buf);

    size_t
    readn         (in_buffer &_buf, error_code &_ec) KNGIN_NOEXCP;

//    size_t
//    writev        (out_vector &_buf);
//
//    size_t
//    writev        (out_vector &_buf, error_code &_ec) KNGIN_NOEXCP;
//
//    size_t
//    readv         (in_vector &_buf);
//
//    size_t
//    readv         (in_vector &_buf, error_code &_ec);

    size_t
    readable      ();

    size_t
    readable      (error_code &_ec) KNGIN_NOEXCP;

    virtual void
    close         ();

    virtual void
    close         (error_code &_ec) KNGIN_NOEXCP;

    filefd_type
    dup           ();

    filefd_type
    dup           (error_code &_ec);

    error_code
    read_error    () KNGIN_NOEXCP;

public:
    void
    set_nonblock  (bool _on);

    void
    set_nonblock  (bool _on, error_code &_ec) KNGIN_NOEXCP;

    void
    set_closeexec (bool _on);

    void
    set_closeexec (bool _on, error_code &_ec) KNGIN_NOEXCP;

    bool
    nonblock      () const;

    bool
    nonblock      (error_code &_ec) const KNGIN_NOEXCP;

public:
    filefd &
    operator =    (filefd_type _fd) KNGIN_NOEXCP;

public:
    static filefd_type invalid_fd;

protected:
    filefd_type m_fd;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FILEFD_H */

#endif /* KNGIN_FLAG_HAVE_FILEFD */

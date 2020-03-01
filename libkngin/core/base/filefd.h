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

typedef int filefd_type;

#define FD_VALID(fd)   (filefd_type(fd) >= 0)
#define FD_INVALID(fd) (filefd_type(fd) < 0)
#define INVALID_FD     (-1);

class filefd : public noncopyable {
public:
    filefd        () = delete;

    explicit
    filefd        (filefd_type _fd) noexcept;

    filefd        (filefd &&_fd) noexcept;

    virtual
    ~filefd       () noexcept;

public:
    bool
    valid         () const noexcept
    { return FD_VALID(m_fd); }

    bool
    invalid       () const noexcept
    { return FD_INVALID(m_fd); }

    filefd_type
    fd            () const noexcept
    { return m_fd; }

    size_t
    write         (out_buffer &_buf);

    size_t
    write         (out_buffer &_buf, error_code &_ec) noexcept;

    size_t
    read          (in_buffer &_buf);

    size_t
    read          (in_buffer &_buf, error_code &_ec) noexcept;

    size_t
    writen        (out_buffer &&_buf);

    size_t
    writen        (out_buffer &&_buf, error_code &_ec) noexcept;

    size_t
    readn         (in_buffer &_buf);

    size_t
    readn         (in_buffer &_buf, error_code &_ec) noexcept;

//    size_t
//    writev        (out_vector &_buf);
//
//    size_t
//    writev        (out_vector &_buf, error_code &_ec) noexcept;
//
//    size_t
//    readv         (in_vector &_buf);
//
//    size_t
//    readv         (in_vector &_buf, error_code &_ec);

    size_t
    readable      ();

    size_t
    readable      (error_code &_ec) noexcept;

    virtual void
    close         ();

    virtual void
    close         (error_code &_ec) noexcept;

    filefd_type
    dup           ();

    filefd_type
    dup           (error_code &_ec);

    error_code
    read_error    () noexcept;

public:
    void
    set_nonblock  (bool _on);

    void
    set_nonblock  (bool _on, error_code &_ec) noexcept;

    void
    set_closeexec (bool _on);

    void
    set_closeexec (bool _on, error_code &_ec) noexcept;

    bool
    nonblock      () const;

    bool
    nonblock      (error_code &_ec) const noexcept;

public:
    filefd &
    operator =    (filefd_type _fd) noexcept;

public:
    static filefd_type invalid_fd;

protected:
    filefd_type m_fd;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FILEFD_H */

#endif /* KNGIN_FLAG_HAVE_FILEFD */

#ifndef KNGIN_FILEFD_H
#define KNGIN_FILEFD_H

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
    filefd  (int _fd) KNGIN_NOEXCP;

    filefd  (filefd &&_fd) KNGIN_NOEXCP;

    virtual
    ~filefd () KNGIN_NOEXCP;

public:
    bool
    valid         () const KNGIN_NOEXCP
    { return FD_VALID(m_fd); }

    bool
    invalid       () const KNGIN_NOEXCP
    { return FD_INVALID(m_fd); }

    int
    fd            () const KNGIN_NOEXCP
    { return m_fd; }

    size_t
    write         (out_buffer &_buf);

    size_t
    write         (out_buffer &_buf, std::error_code &_ec) KNGIN_NOEXCP;

    size_t
    read          (in_buffer &_buf);

    size_t
    read          (in_buffer &_buf, std::error_code &_ec) KNGIN_NOEXCP;

    size_t
    writen        (out_buffer &&_buf);

    size_t
    writen        (out_buffer &&_buf, std::error_code &_ec) KNGIN_NOEXCP;

    size_t
    readn         (in_buffer &_buf);

    size_t
    readn         (in_buffer &_buf, std::error_code &_ec) KNGIN_NOEXCP;

//    size_t
//    writev        (out_vector &_buf);
//
//    size_t
//    writev        (out_vector &_buf, std::error_code &_ec) KNGIN_NOEXCP;
//
//    size_t
//    readv         (in_vector &_buf);
//
//    size_t
//    readv         (in_vector &_buf, std::error_code &_ec);

    size_t
    readable      ();

    size_t
    readable      (std::error_code &_ec) KNGIN_NOEXCP;

    virtual void
    close         ();

    virtual void
    close         (std::error_code &_ec) KNGIN_NOEXCP;

    int
    dup           ();

    int
    dup           (std::error_code &_ec);

    std::error_code
    read_error    () KNGIN_NOEXCP;

public:
    void
    set_nonblock  (bool _on);

    void
    set_nonblock  (bool _on, std::error_code &_ec) KNGIN_NOEXCP;

    void
    set_closeexec (bool _on);

    void
    set_closeexec (bool _on, std::error_code &_ec) KNGIN_NOEXCP;

    bool
    nonblock      () const;

    bool
    nonblock      (std::error_code &_ec) const KNGIN_NOEXCP;

public:
    filefd &
    operator =    (int _fd) KNGIN_NOEXCP;

public:
    static int invalid_fd;

protected:
    int        m_fd;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FILEFD_H */
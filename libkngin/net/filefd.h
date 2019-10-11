#ifndef _FILEFD_H_
#define _FILEFD_H_

#include "define.h"
#include "noncopyable.h"

#define __fd_valid(_fd)   (_fd >= 0)
#define __fd_invalid(_fd) (_fd < 0)

__NAMESPACE_BEGIN

class filefd : public noncopyable {
public:
    filefd  (int _fd);

    virtual
    ~filefd ();

public:
    int
    fd      () const;

    ssize_t
    write   (void *_buf, size_t _nbytes);

    ssize_t
    read    (void *_buf, size_t _nbytes);

    void
    close   ();

protected:
    int m_fd;
};

__NAMESPACE_END

#endif /* _FILEFD_H_ */
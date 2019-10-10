#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include "define.h"
#include "filefd.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class socket : filefd {
public:
    socket (int _sockfd);

    virtual
    ~socket ();

public:
    int m_fd;
};

__NAMESPACE_END

#endif /* _SOCKET_H_ */

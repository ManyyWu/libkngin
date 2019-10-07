#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include "define.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class socket {
public:
    socket() {}//: ::socket() {};

    int
    fd    () {return m_fd;}

public:
    int m_fd;
};

__NAMESPACE_END

#endif /* _SOCKET_H_ */

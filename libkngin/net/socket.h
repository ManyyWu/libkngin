#ifndef _SOCKET_H_
#define _SOCKET_H_

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <memory>
#include <vector>
#include "define.h"
#include "filefd.h"
#include "bits.h"
#include "buffer.h"
#include "epoller_event.h"
#include "event_loop.h"
#include "sockopts.h"
#include "address.h"

__NAMESPACE_BEGIN

class socket : public filefd {
public:
    enum INET_PROTOCOL {
        IPV4_TCP = 0x0, // 00b
        IPV4_UDP = 0x1, // 01b
        IPV6_TCP = 0x2, // 10b
        IPV6_UDP = 0x3, // 11b
    };

public:
    socket      () = delete;

    explicit
    socket      (int _fd);

    explicit
    socket      (INET_PROTOCOL _proto);

    virtual
    ~socket     ();

public:
    bool
    bind        (const address &_addr);

    bool
    listen      (int _max);

public:
    ssize_t
    writev      (const std::vector<buffer> &_buf, size_t _n);

    ssize_t
    readv       (std::vector<buffer> &_buf, size_t _n);

public:
    const sockopts &
    opts        () const { return m_opts; }

protected:
    sockopts m_opts;
};

__NAMESPACE_END

#endif /* _SOCKET_H_ */

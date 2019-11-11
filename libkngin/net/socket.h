#ifndef _SOCKET_H_
#define _SOCKET_H_

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#include <unistd.h>
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
    socket        () = delete;

    explicit
    socket        (int _fd);

    explicit
    socket        (INET_PROTOCOL _proto);

    socket        (socket &&_s);

    virtual
    ~socket       () = default;

public:
    int
    bind          (const address &_addr)
    { return ::bind(m_fd, (const sockaddr *)&(_addr.m_sa), _addr.size()); }
    int
    listen        (int _backlog)
    { return ::listen(m_fd, _backlog); }
    int
    accept        (address &_addr)
    { socklen_t _len = sizeof(_addr.m_sa); return ::accept(m_fd, (sockaddr *)&(_addr.m_sa), &_len); }
    int
    connect       (const address &_addr)
    { return ::connect(m_fd, (const sockaddr *)&(_addr.m_sa), _addr.size()); }

public:
    void
    rd_shutdown   ();

    void
    wr_shutdown   ();

public:
    ssize_t
    send          (buffer &_buf, size_t _nbytes, int _flags);

    ssize_t
    recv          (buffer &_buf, size_t _nbytes, int _flags);

    ssize_t
    sendto        (const address &_addr, buffer &_buf, size_t _nbytes, int _flags);

    ssize_t
    recvfrom      (address &_addr, buffer &_buf, size_t _nbytes, int _flags);

public:
    bool
    localaddr     (address &_addr)
    { socklen_t _len = sizeof(_addr.m_sa); return ::getsockname(m_fd, (sockaddr *)&(_addr.m_sa), &_len); }
    bool
    peeraddr      (address &_addr)
    { socklen_t _len = sizeof(_addr.m_sa); return ::getpeername(m_fd, (sockaddr *)&(_addr.m_sa), &_len); }
};

__NAMESPACE_END

#endif /* _SOCKET_H_ */

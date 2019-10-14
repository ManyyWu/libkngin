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
#include "define.h"
#include "filefd.h"
#include "bits.h"
#include "noncopyable.h"
#include "epoller_event.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

class socket : filefd {
public:
    enum INET_PROTOCOL {
        IPV4_TCP = 0x0, // 00b
        IPV4_UDP = 0x1, // 01b
        IPV6_TCP = 0x2, // 10b
        IPV6_UDP = 0x3, // 11b
    };

public:
    socket           (event_loop *_loop, INET_PROTOCOL _protocol);

    socket           (INET_PROTOCOL _protocol);

    virtual
    ~socket          ();

public:
    bool
    bind             ();

    int
    accept           ();

public:
    bool
    set_reuse_addr   (bool _on = true);

    bool
    set_reuse_port   (bool _on = true);

    bool
    set_keep_alive   (bool _on = true);

    bool
    reuse_port       () const;

    bool
    closeexec        () const;

public:
    epoller_event *
    get_event ();

protected:
    static bool
    check_inet4_addr (const char *_addr);

    static bool
    check_inet6_addr (const char *_addr);

protected:
    event_loop *                   m_loop;

    std::unique_ptr<epoller_event> m_event;
};

__NAMESPACE_END

#endif /* _SOCKET_H_ */

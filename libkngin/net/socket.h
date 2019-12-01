#ifndef _SOCKET_H_
#define _SOCKET_H_

#ifdef _WIN32
#else
#include <sys/socket.h>
#endif
#include <memory>
#include <vector>
#include <system_error>
#include "core/buffer.h"
#include "net/filefd.h"
#include "net/epoller_event.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

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

    socket      (socket &&_s) KNGIN_NOEXP;

    virtual
    ~socket     () KNGIN_NOEXP;

public:
    void
    bind        (const address &_addr);

    void
    bind        (const address &_addr, std::error_code &_ec) KNGIN_NOEXP;

    void
    listen      (int _backlog);

    void
    listen      (int _backlog, std::error_code &_ec) KNGIN_NOEXP;

    int
    accept      (address &_addr);

    int
    accept      (address &_addr, std::error_code &_ec) KNGIN_NOEXP;

    void
    connect     (const address &_addr);

    void
    connect     (const address &_addr, std::error_code &_ec) KNGIN_NOEXP;

public:
    void
    rd_shutdown ();

    void
    rd_shutdown (std::error_code &_ec) KNGIN_NOEXP;

    void
    wr_shutdown ();

    void
    wr_shutdown (std::error_code &_ec) KNGIN_NOEXP;

public:
    size_t
    send        (out_buffer &_buf, int _flags);

    size_t
    send        (out_buffer &_buf, int _flags,
                 std::error_code &_ec) KNGIN_NOEXP;

    size_t
    recv        (in_buffer &_buf, int _flags);

    size_t
    recv        (in_buffer &_buf, int _flags,
                 std::error_code &_ec) KNGIN_NOEXP;

    size_t
    sendto      (const address &_addr, out_buffer &_buf,
                 int _flags);

    size_t
    sendto      (const address &_addr, out_buffer &_buf,
                 int _flags, std::error_code &_ec) KNGIN_NOEXP;

    size_t
    recvfrom    (address &_addr, in_buffer &_buf,
                 int _flags);

    size_t
    recvfrom    (address &_addr, in_buffer &_buf,
                 int _flags, std::error_code &_ec) KNGIN_NOEXP;

public:
    address
    localaddr   ();

    address
    localaddr   (std::error_code &_ec) KNGIN_NOEXP;

    address
    peeraddr    ();

    address
    peeraddr    (std::error_code &_ec) KNGIN_NOEXP;

protected:
    bool m_rd_closed;

    bool m_wr_closed;
};

KNGIN_NAMESPACE_K_END

#endif /* _SOCKET_H_ */

#ifndef KNGIN_SOCKET_H
#define KNGIN_SOCKET_H

#ifdef _WIN32
#else
#include <sys/socket.h>
#endif
#include <memory>
#include <vector>
#include <system_error>
#include "core/base/buffer.h"
#include "core/base/filefd.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

#define KNGIN_PORT_STR_LEN        5

// ipv4: "[%s:%d]" or ipv6: "[[%s]:%d]"
#define KNGIN_CONN_SHORT_NAME_LEN INET6_ADDRSTRLEN + KNGIN_PORT_STR_LEN + 5

// ipv4: "[%s:%d-%s:%d]" or ipv6: "[[%s]:%d-[%s]:%d]"
#define KNGIN_CONN_LONG_NAME_LEN  INET6_ADDRSTRLEN * 2 + KNGIN_PORT_STR_LEN * 2 + 9

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

    socket      (socket &&_s) KNGIN_NOEXCP;

    virtual
    ~socket     () KNGIN_NOEXCP;

public:
    void
    bind        (const address &_addr);

    void
    bind        (const address &_addr, std::error_code &_ec) KNGIN_NOEXCP;

    void
    listen      (int _backlog);

    void
    listen      (int _backlog, std::error_code &_ec) KNGIN_NOEXCP;

    int
    accept      (address &_addr);

    int
    accept      (address &_addr, std::error_code &_ec) KNGIN_NOEXCP;

    void
    connect     (const address &_addr);

    void
    connect     (const address &_addr, std::error_code &_ec) KNGIN_NOEXCP;

    virtual void
    close       ();

    virtual void
    close       (std::error_code &_ec) KNGIN_NOEXCP;

    void
    rd_shutdown ();

    void
    rd_shutdown (std::error_code &_ec) KNGIN_NOEXCP;

    void
    wr_shutdown ();

    void
    wr_shutdown (std::error_code &_ec) KNGIN_NOEXCP;

    bool
    rd_closed   () const KNGIN_NOEXCP
    { return m_rd_closed; }

    bool
    wr_closed   () const KNGIN_NOEXCP
    { return m_wr_closed; }

public:
    size_t
    send        (out_buffer &_buf, int _flags);

    size_t
    send        (out_buffer &_buf, int _flags,
                 std::error_code &_ec) KNGIN_NOEXCP;

    size_t
    recv        (in_buffer &_buf, int _flags);

    size_t
    recv        (in_buffer &_buf, int _flags,
                 std::error_code &_ec) KNGIN_NOEXCP;

    size_t
    sendto      (const address &_addr, out_buffer &_buf,
                 int _flags);

    size_t
    sendto      (const address &_addr, out_buffer &_buf,
                 int _flags, std::error_code &_ec) KNGIN_NOEXCP;

    size_t
    recvfrom    (address &_addr, in_buffer &_buf,
                 int _flags);

    size_t
    recvfrom    (address &_addr, in_buffer &_buf,
                 int _flags, std::error_code &_ec) KNGIN_NOEXCP;

public:
    address
    localaddr   () const;

    address
    localaddr   (std::error_code &_ec) const KNGIN_NOEXCP;

    address
    peeraddr    () const;

    address
    peeraddr    (std::error_code &_ec) const KNGIN_NOEXCP;

    std::string
    name        () const;
/*
    std::string
    full_name   () const;
*/
private:
    bool m_rd_closed;

    bool m_wr_closed;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SOCKET_H */

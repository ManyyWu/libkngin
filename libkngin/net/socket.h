#ifndef KNGIN_SOCKET_H
#define KNGIN_SOCKET_H

#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#endif
#include <memory>
#include <vector>
#include "core/base/define.h"
#include "core/base/buffer.h"
#include "core/base/filefd.h"
#include "core/base/system_error.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

#define KNGIN_PORT_STR_LEN        5

// ipv4: "[%s:%d]" or ipv6: "[[%s]:%d]"
#define KNGIN_CONN_SHORT_NAME_LEN INET6_ADDRSTRLEN + KNGIN_PORT_STR_LEN + 5

// ipv4: "[%s:%d-%s:%d]" or ipv6: "[[%s]:%d-[%s]:%d]"
#define KNGIN_CONN_LONG_NAME_LEN  INET6_ADDRSTRLEN * 2 + KNGIN_PORT_STR_LEN * 2 + 9

#ifdef _WIN32
typedef SOCKET socket_type;
typedef int    len_type;

#define FD_VALID(fd)   (socket_type(fd) >= 0)
#define FD_INVALID(fd) (socket_type(fd) < 0)
#define INVALID_FD     (-1);

#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND

// optimize for windows
class filefd
{
public:
    filefd  (socket_type _fd) KNGIN_NOEXCP
        : m_fd(_fd) {}

public:
    void
    close ();

    void
    close (error_code &_ec) KNGIN_NOEXCP;

public:
    bool
    invalid () const KNGIN_NOEXCP
    { return FD_INVALID(m_fd); }

    bool
    valid   () const KNGIN_NOEXCP
    { return FD_VALID(m_fd); }

    socket_type
    fd      () const KNGIN_NOEXCP
    { return m_fd; } 

protected:
    static socket_type invalid_fd;

protected:
    socket_type m_fd;
};
#else /* _WIN32 */
typedef int socket_type;
typedef size_t len_type;
#endif /* _WIN32 */

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
    socket      (socket_type _fd);

    explicit
    socket      (INET_PROTOCOL _proto);

    socket      (socket &&_s) KNGIN_NOEXCP;

    virtual
    ~socket     () KNGIN_NOEXCP;

public:
    void
    bind        (const address &_addr);

    void
    bind        (const address &_addr, error_code &_ec) KNGIN_NOEXCP;

    void
    listen      (int _backlog);

    void
    listen      (int _backlog, error_code &_ec) KNGIN_NOEXCP;

    socket_type
    accept      (address &_addr);

    socket_type
    accept      (address &_addr, error_code &_ec) KNGIN_NOEXCP;

    void
    connect     (const address &_addr);

    void
    connect     (const address &_addr, error_code &_ec) KNGIN_NOEXCP;

    virtual void
    close       ();

    virtual void
    close       (error_code &_ec) KNGIN_NOEXCP;

    void
    rd_shutdown ();

    void
    rd_shutdown (error_code &_ec) KNGIN_NOEXCP;

    void
    wr_shutdown ();

    void
    wr_shutdown (error_code &_ec) KNGIN_NOEXCP;

    bool
    rd_closed   () const KNGIN_NOEXCP
    { return m_rd_closed; }

    bool
    wr_closed   () const KNGIN_NOEXCP
    { return m_wr_closed; }

#ifdef _WIN32
public:
    size_t
    write       (out_buffer &_buf)
    { return this->send(_buf, 0); }

    size_t
    write       (out_buffer &_buf, error_code &_ec) KNGIN_NOEXCP
    { return this->send(_buf, 0, _ec); }

    size_t
    read        (in_buffer &_buf)
    { return this->recv(_buf, 0); }

    size_t
    read        (in_buffer &_buf, error_code &_ec) KNGIN_NOEXCP
    { return this->recv(_buf, 0, _ec); }
#endif

public:
    size_t
    send        (out_buffer &_buf, int _flags);

    size_t
    send        (out_buffer &_buf, int _flags,
                 error_code &_ec) KNGIN_NOEXCP;

    size_t
    recv        (in_buffer &_buf, int _flags);

    size_t
    recv        (in_buffer &_buf, int _flags,
                 error_code &_ec) KNGIN_NOEXCP;

    size_t
    sendto      (const address &_addr, out_buffer &_buf,
                 int _flags);

    size_t
    sendto      (const address &_addr, out_buffer &_buf,
                 int _flags, error_code &_ec) KNGIN_NOEXCP;

    size_t
    recvfrom    (address &_addr, in_buffer &_buf,
                 int _flags);

    size_t
    recvfrom    (address &_addr, in_buffer &_buf,
                 int _flags, error_code &_ec) KNGIN_NOEXCP;

public:
    address
    localaddr   () const;

    address
    localaddr   (error_code &_ec) const KNGIN_NOEXCP;

    address
    peeraddr    () const;

    address
    peeraddr    (error_code &_ec) const KNGIN_NOEXCP;

    std::string
    name        () const;
/*
    std::string
    full_name   () const;
*/
private:
    bool        m_rd_closed;

    bool        m_wr_closed;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SOCKET_H */

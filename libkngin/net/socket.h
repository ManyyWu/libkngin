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
    filefd  (socket_type fd) noexcept
        : m_fd(fd) {}

public:
    void
    close ();

    void
    close (error_code &ec) noexcept;

public:
    bool
    invalid () const noexcept
    { return FD_INVALID(m_fd); }

    bool
    valid   () const noexcept
    { return FD_VALID(m_fd); }

    socket_type
    fd      () const noexcept
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
    socket      (socket_type fd);

    explicit
    socket      (INET_PROTOCOL proto);

    socket      (socket &&s) noexcept;

    virtual
    ~socket     () noexcept;

public:
    void
    bind        (const address &addr);

    void
    bind        (const address &addr, error_code &ec) noexcept;

    void
    listen      (int backlog);

    void
    listen      (int backlog, error_code &ec) noexcept;

    socket_type
    accept      (address &addr);

    socket_type
    accept      (address &addr, error_code &ec) noexcept;

    void
    connect     (const address &addr);

    void
    connect     (const address &addr, error_code &ec) noexcept;

    virtual void
    close       ();

    virtual void
    close       (error_code &ec) noexcept;

    void
    rd_shutdown ();

    void
    rd_shutdown (error_code &ec) noexcept;

    void
    wr_shutdown ();

    void
    wr_shutdown (error_code &ec) noexcept;

    bool
    rd_closed   () const noexcept
    { return m_rd_closed; }

    bool
    wr_closed   () const noexcept
    { return m_wr_closed; }

#ifdef _WIN32
public:
    size_t
    write       (out_buffer &buf)
    { return this->send(buf, 0); }

    size_t
    write       (out_buffer &buf, error_code &ec) noexcept
    { return this->send(buf, 0, ec); }

    size_t
    read        (in_buffer &buf)
    { return this->recv(buf, 0); }

    size_t
    read        (in_buffer &buf, error_code &ec) noexcept
    { return this->recv(buf, 0, ec); }
#endif

public:
    size_t
    send        (out_buffer &buf, int flags);

    size_t
    send        (out_buffer &buf, int flags,
                 error_code &ec) noexcept;

    size_t
    recv        (in_buffer &buf, int flags);

    size_t
    recv        (in_buffer &buf, int flags,
                 error_code &ec) noexcept;

    size_t
    sendto      (const address &addr, out_buffer &buf,
                 int flags);

    size_t
    sendto      (const address &addr, out_buffer &buf,
                 int flags, error_code &ec) noexcept;

    size_t
    recvfrom    (address &addr, in_buffer &buf,
                 int flags);

    size_t
    recvfrom    (address &addr, in_buffer &buf,
                 int flags, error_code &ec) noexcept;

public:
    address
    localaddr   () const;

    address
    localaddr   (error_code &ec) const noexcept;

    address
    peeraddr    () const;

    address
    peeraddr    (error_code &ec) const noexcept;

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

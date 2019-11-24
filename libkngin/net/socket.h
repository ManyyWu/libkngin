#ifndef _SOCKET_H_
#define _SOCKET_H_

#ifdef _WIN32
#else
#include <socket.h>
#endif
#include <memory>
#include <vector>
#include "filefd.h"
#include "bits.h"
#include "buffer.h"
#include "epoller_event.h"
#include "system_error.h"
#include "event_loop.h"
#include "address.h"

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

    socket      (socket &&_s);

    virtual
    ~socket     () = default;

public:
    void
    bind        (const address &_addr);

    void
    bind        (const address &_addr, std::error_code &_ec);

    void
    listen      (int _backlog);

    void
    listen      (int _backlog, std::error_code &_ec);

    void
    accept      (address &_addr);

    void
    accept      (address &_addr, std::error_code &_ec);

    void
    connect     (const address &_addr);

    void
    connect     (const address &_addr, std::error_code &_ec);

public:
    void
    rd_shutdown ();

    void
    rd_shutdown (std::error_code &_ec);

    void
    wr_shutdown ();

    void
    wr_shutdown (std::error_code &_ec);

public:
    size_t
    send        (buffer &_buf, size_t &_nbytes, int _flags);

    size_t
    send        (buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec);

    size_t
    recv        (buffer &_buf, size_t &_nbytes, int _flags);

    size_t
    recv        (buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec);

    size_t
    sendto      (const address &_addr, buffer &_buf, size_t &_nbytes, int _flags);

    size_t
    sendto      (const address &_addr, buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec);

    size_t
    recvfrom    (address &_addr, buffer &_buf, size_t &_nbytes, int _flags);

    size_t
    recvfrom    (address &_addr, buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec);

public:
    address
    localaddr   ();

    address
    localaddr   (std::error_code &_ec);

    address
    peeraddr    ();

    address
    peeraddr    (std::error_code &_ec);
protected:
    bool m_rd_closed;

    bool m_wr_closed;
};

KNGIN_NAMESPACE_K_END

#endif /* _SOCKET_H_ */

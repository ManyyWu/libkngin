#ifndef _SOCKET_H_
#define _SOCKET_H_

#ifdef _WIN32
#else
#include <socket.h>
#endif
#include <memory>
#include <vector>
#include "core/filefd.h"
#include "core/bits.h"
#include "net/buffer.h"
#include "net/epoller_event.h"
#include "net/system_error.h"
#include "net/event_loop.h"
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
    socket      (int _fd) KNGIN_EXP;

    explicit
    socket      (INET_PROTOCOL _proto) KNGIN_EXP;

    socket      (socket &&_s) KNGIN_NOEXP;

    virtual
    ~socket     () KNGIN_NOEXP;

public:
    void
    bind        (const address &_addr) KNGIN_EXP;

    void
    bind        (const address &_addr, std::error_code &_ec) KNGIN_NOEXP;

    void
    listen      (int _backlog) KNGIN_EXP;

    void
    listen      (int _backlog, std::error_code &_ec) KNGIN_NOEXP;

    int
    accept      (address &_addr) KNGIN_EXP;

    int
    accept      (address &_addr, std::error_code &_ec) KNGIN_NOEXP;

    void
    connect     (const address &_addr) KNGIN_EXP;

    void
    connect     (const address &_addr, std::error_code &_ec) KNGIN_NOEXP;

public:
    void
    rd_shutdown () KNGIN_EXP;

    void
    rd_shutdown (std::error_code &_ec) KNGIN_NOEXP;

    void
    wr_shutdown () KNGIN_EXP;

    void
    wr_shutdown (std::error_code &_ec) KNGIN_NOEXP;

public:
    size_t
    send        (buffer &_buf, size_t &_nbytes, int _flags) KNGIN_EXP;

    size_t
    send        (buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec) KNGIN_NOEXP;

    size_t
    recv        (buffer &_buf, size_t &_nbytes, int _flags) KNGIN_EXP;

    size_t
    recv        (buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec) KNGIN_NOEXP;

    size_t
    sendto      (const address &_addr, buffer &_buf, size_t &_nbytes, int _flags) KNGIN_EXP;

    size_t
    sendto      (const address &_addr, buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec) KNGIN_NOEXP;

    size_t
    recvfrom    (address &_addr, buffer &_buf, size_t &_nbytes, int _flags) KNGIN_EXP;

    size_t
    recvfrom    (address &_addr, buffer &_buf, size_t &_nbytes, int _flags,
                 std::error_code &_ec) KNGIN_NOEXP;

public:
    address
    localaddr   () KNGIN_EXP;

    address
    localaddr   (std::error_code &_ec) KNGIN_NOEXP;

    address
    peeraddr    () KNGIN_EXP;

    address
    peeraddr    (std::error_code &_ec) KNGIN_NOEXP;

protected:
    bool m_rd_closed;

    bool m_wr_closed;
};

KNGIN_NAMESPACE_K_END

#endif /* _SOCKET_H_ */

#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#ifdef _WIN32
#else
#include <netinet/in.h>
#endif
#include <array>
#include <cassert>
#include <cstring>
#include "core/define.h"

KNGIN_NAMESPACE_K_BEGIN

class address {
public:
    union in_addr {
        struct ::in_addr  v4;
        struct ::in6_addr v6;
    };

    union sockaddr {
        struct ::sockaddr_in  v4;
        struct ::sockaddr_in6 v6;
    };

public:
    address    () = default;
    address    (const struct ::sockaddr_in &_sa) KNGIN_NOEXCP
    { assert(AF_INET == _sa.sin_family); m_sa.v4 = _sa; }
    address    (const struct ::sockaddr_in &&_sa) KNGIN_NOEXCP
    { assert(AF_INET == _sa.sin_family); m_sa.v4 = _sa; }
    address    (const struct ::sockaddr_in6 &_sa) KNGIN_NOEXCP
    { assert(AF_INET6 == _sa.sin6_family); m_sa.v6 = _sa; }
    address    (const struct ::sockaddr_in6 &&_sa) KNGIN_NOEXCP
    { assert(AF_INET6 == _sa.sin6_family); m_sa.v6 = _sa; }
    address    (const address &_sa) KNGIN_NOEXCP
    { m_sa = _sa.m_sa; }
    address    (const address &&_sa) KNGIN_NOEXCP
    { m_sa = _sa.m_sa; }
    ~address   () = default;

public:
    bool
    inet6      () const;

    int
    size       () const KNGIN_NOEXCP;

    std::string
    addrstr    () const;

    uint16_t
    port       () const KNGIN_NOEXCP;

public:
    address &
    operator = (const struct ::sockaddr_in &_sa) KNGIN_NOEXCP
    { assert(AF_INET == _sa.sin_family); m_sa.v4 = _sa; return *this; }
    address &
    operator = (const struct ::sockaddr_in &&_sa) KNGIN_NOEXCP
    { assert(AF_INET == _sa.sin_family); m_sa.v4 = _sa; return *this; }
    address &
    operator = (const struct ::sockaddr_in6 &_sa) KNGIN_NOEXCP
    { assert(AF_INET6 == _sa.sin6_family); m_sa.v6 = _sa; return *this; }
    address &
    operator = (const struct ::sockaddr_in6 &&_sa) KNGIN_NOEXCP
    { assert(AF_INET6 == _sa.sin6_family); m_sa.v6 = _sa; return *this; }
    address &
    operator = (const address &_sa) KNGIN_NOEXCP
    { m_sa = _sa.m_sa; return *this; }
    address &
    operator = (const address &&_sa) KNGIN_NOEXCP
    { m_sa = _sa.m_sa; return *this; }

public:
    const address::sockaddr &
    sa         () const KNGIN_NOEXCP
    { return m_sa; }

    address::sockaddr &
    sa         () KNGIN_NOEXCP
    { return m_sa; }

public:
    static bool
    addrstr2addr (const std::string &_addrstr, uint16_t _port, bool _v6,
                  address &_addr) KNGIN_NOEXCP;

    static bool
    addrstr2addr (const std::string &&_addrstr, uint16_t _port, bool _v6,
                  address &_addr) KNGIN_NOEXCP;

public:
    static bool
    check_inet_addrstr  (const std::string &_addrstr) KNGIN_NOEXCP;

    static bool
    check_inet_addrstr  (const std::string &&_addrstr) KNGIN_NOEXCP;

    static bool
    check_inet6_addrstr (const std::string &_addrstr) KNGIN_NOEXCP;

    static bool
    check_inet6_addrstr (const std::string &&_addrstr) KNGIN_NOEXCP;

protected:
    address::sockaddr m_sa;

protected:
    friend class socket;
};

KNGIN_NAMESPACE_K_END

#endif /* _ADDRESS_H_ */

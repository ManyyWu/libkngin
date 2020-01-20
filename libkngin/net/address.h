#ifndef KNGIN_ADDRESS_H
#define KNGIN_ADDRESS_H

#ifdef _WIN32
#else
#include <netinet/in.h>
#endif
#include <array>
#include <cassert>
#include <cstring>
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

#define KEY_SIZE sizeof(struct ::in6_addr) + sizeof(::in_port_t) + 1

class address {
    friend class socket;

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
    address    (const std::string &_addrstr, uint16_t _port, bool _v6);
    ~address   () = default;

public:
    int
    family     () const KNGIN_NOEXCP
    { return m_sa.v4.sin_family; }

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
    bool
    operator == (const address &_sa) KNGIN_NOEXCP
    { return !::memcmp(&m_sa, &_sa, inet6() ? sizeof(sockaddr::v6) : sizeof(sockaddr::v4)); }
    bool
    operator != (const address &_sa) KNGIN_NOEXCP
    { return ::memcmp(&m_sa, &_sa, inet6() ? sizeof(sockaddr::v6) : sizeof(sockaddr::v4)); }

public:
    const address::sockaddr &
    sa         () const KNGIN_NOEXCP
    { return m_sa; }

    address::sockaddr &
    sa         () KNGIN_NOEXCP
    { return m_sa; }

public:
    bool
    is_ipv4_mapped         () const KNGIN_NOEXCP;

public:
    static bool
    is_valid_inet_addrstr  (const std::string &_addrstr);

    static bool
    is_valid_inet6_addrstr (const std::string &_addrstr);

public:
    std::string
    key                    () const;

protected:
    address::sockaddr m_sa;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ADDRESS_H */

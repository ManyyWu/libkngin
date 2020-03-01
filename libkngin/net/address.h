#ifndef KNGIN_ADDRESS_H
#define KNGIN_ADDRESS_H

#ifdef _WIN32
#include <ws2ipdef.h>
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
    address    (const struct ::sockaddr_in &sa) noexcept
    { assert(AF_INET == sa.sin_family); sa_.v4 = sa; }
    address    (const struct ::sockaddr_in &&sa) noexcept
    { assert(AF_INET == sa.sin_family); sa_.v4 = sa; }
    address    (const struct ::sockaddr_in6 &sa) noexcept
    { assert(AF_INET6 == sa.sin6_family); sa_.v6 = sa; }
    address    (const struct ::sockaddr_in6 &&sa) noexcept
    { assert(AF_INET6 == sa.sin6_family); sa_.v6 = sa; }
    address    (const address &sa) noexcept
    { sa_ = sa.sa_; }
    address    (const address &&sa) noexcept
    { sa_ = sa.sa_; }
    address    (const std::string &addrstr, uint16_t port, bool v6);
    ~address   () = default;

public:
    int
    family     () const noexcept
    { return sa_.v4.sin_family; }

    bool
    inet6      () const;

    int
    size       () const noexcept;

    std::string
    addrstr    () const;

    uint16_t
    port       () const noexcept;

public:
    address &
    operator = (const struct ::sockaddr_in &sa) noexcept
    { assert(AF_INET == sa.sin_family); sa_.v4 = sa; return *this; }
    address &
    operator = (const struct ::sockaddr_in &&sa) noexcept
    { assert(AF_INET == sa.sin_family); sa_.v4 = sa; return *this; }
    address &
    operator = (const struct ::sockaddr_in6 &sa) noexcept
    { assert(AF_INET6 == sa.sin6_family); sa_.v6 = sa; return *this; }
    address &
    operator = (const struct ::sockaddr_in6 &&sa) noexcept
    { assert(AF_INET6 == sa.sin6_family); sa_.v6 = sa; return *this; }
    address &
    operator = (const address &sa) noexcept
    { sa_ = sa.sa_; return *this; }
    address &
    operator = (const address &&sa) noexcept
    { sa_ = sa.sa_; return *this; }

public:
    bool
    operator == (const address &sa) noexcept
    { return !::memcmp(&sa_, &sa, inet6() ? sizeof(sockaddr::v6) : sizeof(sockaddr::v4)); }
    bool
    operator != (const address &sa) noexcept
    { return ::memcmp(&sa_, &sa, inet6() ? sizeof(sockaddr::v6) : sizeof(sockaddr::v4)); }

public:
    const address::sockaddr &
    sa         () const noexcept
    { return sa_; }

    address::sockaddr &
    sa         () noexcept
    { return sa_; }

public:
    bool
    is_ipv4_mapped         () const noexcept;

public:
    static bool
    is_valid_inet_addrstr  (const std::string &addrstr);

    static bool
    is_valid_inet6_addrstr (const std::string &addrstr);

public:
    std::string
    key                    () const;

protected:
    address::sockaddr sa_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ADDRESS_H */

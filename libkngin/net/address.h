#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#ifdef _WIN32
#else
#include <netinet/in.h>
#endif
#include <array>
#include <cstring>

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
    address    (const ::sockaddr &_sa) KNGIN_NOEXP
    { ::memcpy(&m_sa, &_sa, sizeof(::sockaddr)); }
    address    (const ::sockaddr &&_sa) KNGIN_NOEXP
    { ::memcpy(&m_sa, &_sa, sizeof(::sockaddr)); }
    address    (const ::sockaddr_in &_sa) KNGIN_NOEXP
    { m_sa.v4 = _sa; }
    address    (const ::sockaddr_in &&_sa) KNGIN_NOEXP
    { m_sa.v4 = _sa; }
    address    (const ::sockaddr_in6 &_sa) KNGIN_NOEXP
    { m_sa.v6 = _sa; }
    address    (const ::sockaddr_in6 &&_sa) KNGIN_NOEXP
    { m_sa.v6 = _sa; }
    address    (const address &_sa) KNGIN_NOEXP
    { m_sa = _sa.m_sa; }
    address    (const address &&_sa) KNGIN_NOEXP
    { m_sa = _sa.m_sa; }
    ~address   () = default;

public:
    bool
    inet6      () const;

    int
    size       () const KNGIN_NOEXP;

    std::string
    addrstr    () const;

    uint16_t
    port       () const KNGIN_NOEXP;

public:
    address &
    operator = (const ::sockaddr &_sa) KNGIN_NOEXP
    { ::memcpy(&m_sa, &_sa, sizeof(address::sockaddr)); return *this; }
    address &
    operator = (const ::sockaddr &&_sa) KNGIN_NOEXP
    { ::memcpy(&m_sa, &_sa, sizeof(address::sockaddr)); return *this; }
    address &
    operator = (const ::sockaddr_in &_sa) KNGIN_NOEXP
    { m_sa.v4 = _sa; return *this; }
    address &
    operator = (const ::sockaddr_in &&_sa) KNGIN_NOEXP
    { m_sa.v4 = _sa; return *this; }
    address &
    operator = (const ::sockaddr_in6 &_sa) KNGIN_NOEXP
    { m_sa.v6 = _sa; return *this; }
    address &
    operator = (const ::sockaddr_in6 &&_sa) KNGIN_NOEXP
    { m_sa.v6 = _sa; return *this; }
    address &
    operator = (const address &_sa) KNGIN_NOEXP
    { m_sa = _sa.m_sa; return *this; }
    address &
    operator = (const address &&_sa) KNGIN_NOEXP
    { m_sa = _sa.m_sa; return *this; }

public:
    const address::sockaddr &
    sa         () const KNGIN_NOEXP
    { return m_sa; }

    address::sockaddr &
    sa         () KNGIN_NOEXP
    { return m_sa; }

public:
    static bool
    addrstr2addr (const std::string &_addrstr, uint16_t _port, address &_addr) KNGIN_NOEXP;

    static bool
    addrstr2addr (const std::string &&_addrstr, uint16_t _port, address &_addr) KNGIN_NOEXP;

public:
    static bool
    check_inet_addrstr  (const std::string &_addrstr) KNGIN_NOEXP;

    static bool
    check_inet_addrstr  (const std::string &&_addrstr) KNGIN_NOEXP;

    static bool
    check_inet6_addrstr (const std::string &_addrstr) KNGIN_NOEXP;

    static bool
    check_inet6_addrstr (const std::string &&_addrstr) KNGIN_NOEXP;

protected:
    address::sockaddr m_sa;

protected:
    friend class socket;
};

KNGIN_NAMESPACE_K_END

#endif /* _ADDRESS_H_ */

#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include <netinet/in.h>
#include <utility>
#include <array>

__NAMESPACE_BEGIN

union __address {
    struct in_addr  addr_inet4;
    struct in6_addr addr_inet6;
};

union __sockaddr {
    struct sockaddr_in  sa_in;
    struct sockaddr_in6 sa_in6;
};

typedef std::array<char, INET_ADDRSTRLEN>  inet_addrstr;
typedef std::array<char, INET6_ADDRSTRLEN> inet6_addrstr;

class address {
public:
    address    ()                         {  }
    address    (const sockaddr_in &_sa)   { m_sa.sa_in = _sa; }
    address    (const sockaddr_in &&_sa)  { m_sa.sa_in = _sa; }
    address    (const sockaddr_in6 &_sa)  { m_sa.sa_in6 = _sa; }
    address    (const sockaddr_in6 &&_sa) { m_sa.sa_in6 = _sa; }
    address    (const address &_sa)       { m_sa = _sa.m_sa; }
    address    (const address &&_sa)      { m_sa = _sa.m_sa; }
    ~address   () = default;

public:
    bool
    inet6      () const;

    int
    size       () const;

    const char *
    addrstr    (inet_addrstr &_s) const;

    const char *
    addrstr    (inet6_addrstr &_s) const;

    uint16_t
    port       () const;

public:
    address &
    operator = (const sockaddr_in &_sa)   { m_sa.sa_in = _sa; return *this; }
    address &
    operator = (const sockaddr_in &&_sa)  { m_sa.sa_in = _sa; return *this; }
    address &
    operator = (const sockaddr_in6 &_sa)  { m_sa.sa_in6 = _sa; return *this; }
    address &
    operator = (const sockaddr_in6 &&_sa) { m_sa.sa_in6 = _sa; return *this; }
    address &
    operator = (const address &_sa)       { m_sa = _sa.m_sa; return *this; }
    address &
    operator = (const address &&_sa)      { m_sa = _sa.m_sa; return *this; }

public:
    const __sockaddr &
    sa         () const                   { return m_sa; }

    __sockaddr &
    sa         ()                         { return m_sa; }

public:
    static bool
    str2sockaddr   (const inet_addrstr &_addrstr, uint16_t _port, address &_addr);

    static bool
    str2sockaddr   (const inet6_addrstr &_addrstr, uint16_t _port, address &_addr);

    static bool
    str2sockaddr   (const inet_addrstr &&_addrstr, uint16_t _port, address &_addr);

    static bool
    str2sockaddr   (const inet6_addrstr &&_addrstr, uint16_t _port, address &_addr);

public:
    static bool
    check_sockaddr (const inet_addrstr &_addrstr);

    static bool
    check_sockaddr (const inet6_addrstr &_addrstr);

    static bool
    check_sockaddr (const inet_addrstr &&_addrstr);

    static bool
    check_sockaddr (const inet6_addrstr &&_addrstr);

protected:
    __sockaddr m_sa;

protected:
    friend class socket;
    friend class connnection;
};

__NAMESPACE_END

#endif /* _ADDRESS_H_ */

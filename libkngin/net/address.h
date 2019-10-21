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
    address  () = delete;

    explicit
    address  (const sockaddr_in &_sa)   { m_sa.sa_in = _sa; };

    explicit
    address  (const  sockaddr_in &&_sa) { m_sa.sa_in = _sa; }

    explicit
    address  (const sockaddr_in6 &_sa)  { m_sa.sa_in6 = _sa; }

    explicit
    address  (const sockaddr_in6 &&_sa) { m_sa.sa_in6 = _sa; }

    explicit
    address  (const address &_sa)       { m_sa = _sa.m_sa; }

    explicit
    address  (const address &&_sa)      { m_sa = _sa.m_sa; }

    ~address () = delete;

public:
    bool
    inet6            () const;

    bool
    addr             (inet6_addrstr &_s) const;

    uint16_t
    port             () const { return inet6() ? m_sa.sa_in.sin_port : m_sa.sa_in6.sin6_port; }

public:
    static bool
    check_inet4_addr (const char *_addr);

    static bool
    check_inet6_addr (const char *_addr);

protected:
    const __sockaddr &
    sa               () const { return m_sa; }

protected:
    __sockaddr m_sa;

protected:
    friend class socket;
    friend class connnection;
};

__NAMESPACE_END

#endif /* _ADDRESS_H_ */

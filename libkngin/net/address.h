#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include <netinet/in.h>
#include <utility>

union __inet_addr {
    struct in_addr  addr_inet4;
    struct in6_addr addr_inet6;
};

union __addr_str {
    char str_inet4[INET_ADDRSTRLEN];
    char str_inet6[INET6_ADDRSTRLEN];
};

class inet_addr {
public:
    inet_addr        () = delete;

    explicit
    inet_addr        (const in_addr &_addr)  { m_addr.addr_inet4 = _addr; }

    explicit
    inet_addr        (in_addr &&_addr)       { m_addr.addr_inet4 = _addr; }

    explicit
    inet_addr        (const in6_addr &_addr) { m_addr.addr_inet6 = _addr; }

    explicit
    inet_addr        (in6_addr &&_addr)      { m_addr.addr_inet6 = _addr; }

    ~inet_addr       () = delete;

public:
    static bool
    check_inet4_addr (const char *_addr);

    static bool
    check_inet6_addr (const char *_addr);

protected:
    __inet_addr m_addr;
};

#endif /* _ADDRESS_H_ */

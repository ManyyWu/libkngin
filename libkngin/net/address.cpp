#ifdef _WIN32
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "define.h"
#include "exception.h"
#include "address.h"
#include "common.h"

__NAMESPACE_BEGIN

bool
address::inet6 () const
{
    struct sockaddr *_addr = (sockaddr *)&(m_sa.sa_in6);
    if (AF_INET == _addr->sa_family)
        return false;
    else if (AF_INET6 == _addr->sa_family)
        return true;
    else
        throw exception("invalid address family");
}

int
address::size () const
{
    return (inet6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in));
}

const char *
address::addrstr (inet_addrstr &_s) const
{
    kassert(!inet6());
    return ::inet_ntop(AF_INET, &(m_sa.sa_in.sin_addr), _s.data(), sizeof(sockaddr_in));
}

const char *
address::addrstr (inet6_addrstr &_s) const
{
    kassert(inet6());
    return ::inet_ntop(AF_INET6, &(m_sa.sa_in6.sin6_addr), _s.data(), sizeof(sockaddr_in6));
}

uint16_t
address::port () const
{
    return ::ntohs(inet6() ? m_sa.sa_in.sin_port : m_sa.sa_in6.sin6_port);
}

bool
address::str2sockaddr (const inet_addrstr &_addrstr, uint16_t _port, address &_addr)
{
    _addr.m_sa.sa_in.sin_port = ::htons(_port);
    _addr.m_sa.sa_in.sin_family = AF_INET;
    return ::inet_pton(AF_INET, _addrstr.data(), &(_addr.m_sa.sa_in.sin_addr));
}

bool
address::str2sockaddr (const inet6_addrstr &_addrstr, uint16_t _port, address &_addr)
{
    _addr.m_sa.sa_in6.sin6_port = ::htons(_port);
    _addr.m_sa.sa_in6.sin6_family = AF_INET6;
    return ::inet_pton(AF_INET, _addrstr.data(), &(_addr.m_sa.sa_in6.sin6_addr));
}


bool
address::str2sockaddr (const inet_addrstr &&_addrstr, uint16_t _port, address &_addr)
{
    _addr.m_sa.sa_in.sin_port = ::htons(_port);
    _addr.m_sa.sa_in.sin_family = AF_INET;
    return ::inet_pton(AF_INET, _addrstr.data(), &(_addr.m_sa.sa_in.sin_addr));
}

bool
address::str2sockaddr (const inet6_addrstr &&_addrstr, uint16_t _port, address &_addr)
{
    _addr.m_sa.sa_in6.sin6_port = htons(_port);
    _addr.m_sa.sa_in6.sin6_family = AF_INET6;
    return ::inet_pton(AF_INET, _addrstr.data(), &(_addr.m_sa.sa_in6.sin6_addr));
}

bool
address::check_sockaddr (const inet_addrstr &_addrstr)
{
    sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_sockaddr (const inet6_addrstr &_addrstr)
{
    sockaddr_in6 _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_sockaddr (const inet_addrstr &&_addrstr)
{
    sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_sockaddr (const inet6_addrstr &&_addrstr)
{
    sockaddr_in6 _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}
__NAMESPACE_END

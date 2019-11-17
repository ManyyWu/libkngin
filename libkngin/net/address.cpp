#ifdef _WIN32
#else
#include <arpa/inet.h>
#endif
#include "exception.h"
#include "address.h"
#include "common.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/address.cpp"

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
        throw k::exception("unsupported address family");
}

int
address::size () const
{
    return (inet6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in));
}

std::string
address::addrstr () const
{
    char _buf[INET_ADDRSTRLEN];
    return std::string(::inet_ntop(AF_INET, &(m_sa.sa_in.sin_addr),
                                   _buf, sizeof(sockaddr_in))
                       ? _buf : "");
}

uint16_t
address::port () const
{
    return ::ntohs(inet6() ? m_sa.sa_in.sin_port : m_sa.sa_in6.sin6_port);
}

bool
address::str2sockaddr (const std::string &_addrstr, uint16_t _port, address &_addr)
{
    ::bzero(&_addr.m_sa, sizeof(_addr.m_sa));
    _addr.m_sa.sa_in.sin_port = ::htons(_port);
    _addr.m_sa.sa_in.sin_family = AF_INET;
    return ::inet_pton(AF_INET, _addrstr.c_str(), &(_addr.m_sa.sa_in.sin_addr));
}

bool
address::str2sockaddr (const std::string &&_addrstr, uint16_t _port, address &_addr)
{
    ::bzero(&_addr.m_sa, sizeof(_addr.m_sa));
    _addr.m_sa.sa_in.sin_port = ::htons(_port);
    _addr.m_sa.sa_in.sin_family = AF_INET;
    return ::inet_pton(AF_INET, _addrstr.data(), &(_addr.m_sa.sa_in.sin_addr));
}

bool
address::check_inet_addrstr (const std::string &_addrstr)
{
    sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_inet_addrstr (const std::string &&_addrstr)
{
    sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_inet6_addrstr (const std::string &_addrstr)
{
    sockaddr_in _sa;
    return ::inet_pton(AF_INET6, _addrstr.data(), &_sa);
}

bool
address::check_inet6_addrstr (const std::string &&_addrstr)
{
    sockaddr_in _sa;
    return ::inet_pton(AF_INET6, _addrstr.data(), &_sa);
}
__NAMESPACE_END

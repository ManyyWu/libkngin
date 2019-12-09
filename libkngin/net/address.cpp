#ifdef _WIN32
#else
#include <arpa/inet.h>
#endif
#include "core/exception.h"
#include "core/common.h"
#include "net/address.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/address.cpp"

KNGIN_NAMESPACE_K_BEGIN

bool
address::inet6 () const
{
    struct ::sockaddr *_addr = (struct ::sockaddr *)&m_sa.v6;
    if (AF_INET == _addr->sa_family)
        return false;
    else if (AF_INET6 == _addr->sa_family)
        return true;
    else
        throw k::exception("unsupported address family");
}

int
address::size () const KNGIN_NOEXCP
{
    return (inet6() ? sizeof(struct ::sockaddr_in6) : sizeof(struct ::sockaddr_in));
}

std::string
address::addrstr () const
{
    char _buf[INET_ADDRSTRLEN];
    return std::string(::inet_ntop(AF_INET, &m_sa.v4.sin_addr,
                                   _buf, sizeof(sockaddr_in))
                       ? _buf : "");
}

uint16_t
address::port () const KNGIN_NOEXCP
{
    return ::ntohs(inet6() ? m_sa.v4.sin_port : m_sa.v6.sin6_port);
}

bool
address::addrstr2addr (const std::string &_addrstr,
                       uint16_t _port, address &_addr) KNGIN_NOEXCP
{
    ::bzero(&_addr.m_sa, sizeof(_addr.m_sa));
    _addr.m_sa.v4.sin_port = ::htons(_port);
    _addr.m_sa.v4.sin_family = AF_INET;
    return ::inet_pton(AF_INET, _addrstr.c_str(), &_addr.m_sa.v4.sin_addr);
}

bool
address::addrstr2addr (const std::string &&_addrstr,
                       uint16_t _port, address &_addr) KNGIN_NOEXCP
{
    ::bzero(&_addr.m_sa, sizeof(_addr.m_sa));
    _addr.m_sa.v4.sin_port = ::htons(_port);
    _addr.m_sa.v4.sin_family = AF_INET;
    return ::inet_pton(AF_INET, _addrstr.data(), &_addr.m_sa.v4.sin_addr);
}

bool
address::check_inet_addrstr (const std::string &_addrstr) KNGIN_NOEXCP
{
    ::sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_inet_addrstr (const std::string &&_addrstr) KNGIN_NOEXCP
{
    ::sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_inet6_addrstr (const std::string &_addrstr) KNGIN_NOEXCP
{
    ::sockaddr_in _sa;
    return ::inet_pton(AF_INET6, _addrstr.data(), &_sa);
}

bool
address::check_inet6_addrstr (const std::string &&_addrstr) KNGIN_NOEXCP
{
    ::sockaddr_in _sa;
    return ::inet_pton(AF_INET6, _addrstr.data(), &_sa);
}
KNGIN_NAMESPACE_K_END

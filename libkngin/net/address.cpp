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
    char _buf[INET6_ADDRSTRLEN];
    return std::string(::inet_ntop(inet6() ? AF_INET6 : AF_INET, 
                                   inet6() ? (void *)&m_sa.v6.sin6_addr : (void *)&m_sa.v4.sin_addr,
                                   _buf, sizeof(_buf)
                                   ) ? _buf : "");
}

uint16_t
address::port () const KNGIN_NOEXCP
{
    return ::ntohs(inet6() ? m_sa.v4.sin_port : m_sa.v6.sin6_port);
}

bool
address::addrstr2addr (const std::string &_addrstr, uint16_t _port, bool _v6,
                       address &_addr) KNGIN_NOEXCP
{
    if (_v6) {
        _addr.m_sa.v6.sin6_port = ::htons(_port);
        _addr.m_sa.v6.sin6_family = AF_INET6;
    } else {
        _addr.m_sa.v4.sin_port = ::htons(_port);
        _addr.m_sa.v4.sin_family = AF_INET;
    }
    return !::inet_pton(_v6 ? AF_INET6 : AF_INET, _addrstr.c_str(), &_addr.sa());
}

bool
address::addrstr2addr (const std::string &&_addrstr, uint16_t _port, bool _v6,
                       address &_addr) KNGIN_NOEXCP
{
    if (_v6) {
        _addr.m_sa.v6.sin6_port = ::htons(_port);
        _addr.m_sa.v6.sin6_family = AF_INET6;
    } else {
        _addr.m_sa.v4.sin_port = ::htons(_port);
        _addr.m_sa.v4.sin_family = AF_INET;
    }
    return !::inet_pton(_v6 ? AF_INET6 : AF_INET, _addrstr.c_str(), &_addr.sa());
}

bool
address::check_inet_addrstr (const std::string &_addrstr) KNGIN_NOEXCP
{
    struct ::sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_inet_addrstr (const std::string &&_addrstr) KNGIN_NOEXCP
{
    struct ::sockaddr_in _sa;
    return ::inet_pton(AF_INET, _addrstr.data(), &_sa);
}

bool
address::check_inet6_addrstr (const std::string &_addrstr) KNGIN_NOEXCP
{
    struct ::sockaddr_in _sa;
    return ::inet_pton(AF_INET6, _addrstr.data(), &_sa);
}

bool
address::check_inet6_addrstr (const std::string &&_addrstr) KNGIN_NOEXCP
{
    struct ::sockaddr_in _sa;
    return ::inet_pton(AF_INET6, _addrstr.data(), &_sa);
}
KNGIN_NAMESPACE_K_END

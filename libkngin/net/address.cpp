#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <arpa/inet.h>
#endif
#include "core/base/exception.h"
#include "core/base/system_error.h"
#include "net/address.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/address.cpp"

KNGIN_NAMESPACE_K_BEGIN

address::address (const std::string &_addrstr, uint16_t _port, bool _v6)
{
    if (_v6) {
        m_sa.v6.sin6_port = ::htons(_port);
        m_sa.v6.sin6_family = AF_INET6;
    } else {
        m_sa.v4.sin_port = ::htons(_port);
        m_sa.v4.sin_family = AF_INET;
    }
    int _ret = ::inet_pton(_v6 ? AF_INET6 : AF_INET,
                           _addrstr.c_str(),
                           _v6 ? (void *)&m_sa.v6.sin6_addr : (void *)&m_sa.v4.sin_addr);
    if (_ret < 0)
        throw k::system_error("::inet_pton() error");
}

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
    const char *_ret =
        ::inet_ntop(inet6() ? AF_INET6 : AF_INET,
                    inet6() ? (void *)&m_sa.v6.sin6_addr : (void *)&m_sa.v4.sin_addr,
                    _buf, sizeof(_buf)
                    );
    if (!_ret)
        throw k::system_error("::inet_ntop() error");
    return std::string(_buf);
}

uint16_t
address::port () const KNGIN_NOEXCP
{
    return ::ntohs(inet6() ? m_sa.v4.sin_port : m_sa.v6.sin6_port);
}

bool
address::is_ipv4_mapped () const KNGIN_NOEXCP
{
    assert(inet6());
    return IN6_IS_ADDR_V4MAPPED(&m_sa.v6.sin6_addr);
}

bool
address::is_valid_inet_addrstr (const std::string &_addrstr)
{
    struct ::sockaddr_in _sa;
    int _ret = ::inet_pton(AF_INET, _addrstr.data(), &_sa);
    if (_ret < 0)
        throw k::system_error("::inet_pton() error");
    return _ret;
}

bool
address::is_valid_inet6_addrstr (const std::string &_addrstr)
{
    struct ::sockaddr_in _sa;
    int _ret = ::inet_pton(AF_INET6, _addrstr.data(), &_sa);
    if (_ret < 0)
        throw k::system_error("::inet_pton() error");
    return _ret;
}

std::string
address::key () const
{
    if (inet6())
        return std::string((char *)&m_sa.v6.sin6_addr, sizeof(struct ::in6_addr)
                           ).append(std::to_string(m_sa.v6.sin6_port));
    else
        return std::string((char *)&m_sa.v4.sin_addr, sizeof(struct ::in_addr)
                           ).append(std::to_string(m_sa.v4.sin_port));
}

KNGIN_NAMESPACE_K_END

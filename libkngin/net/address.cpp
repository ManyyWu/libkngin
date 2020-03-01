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

address::address (const std::string &addrstr, uint16_t port, bool v6)
{
    if (v6) {
        m_sa.v6.sin6_port = ::htons(port);
        m_sa.v6.sin6_family = AF_INET6;
    } else {
        m_sa.v4.sin_port = ::htons(port);
        m_sa.v4.sin_family = AF_INET;
    }
    int ret = ::inet_pton(v6 ? AF_INET6 : AF_INET,
                           addrstr.c_str(),
                           v6 ? (void *)&m_sa.v6.sin6_addr : (void *)&m_sa.v4.sin_addr);
    if (ret < 0)
        throw k::system_error("::inet_pton() error");
}

bool
address::inet6 () const
{
    struct ::sockaddr *addr = (struct ::sockaddr *)&m_sa.v6;
    if (AF_INET == addr->sa_family)
        return false;
    else if (AF_INET6 == addr->sa_family)
        return true;
    else
        throw k::exception("unsupported address family");
}

int
address::size () const noexcept
{
    return (inet6() ? sizeof(struct ::sockaddr_in6) : sizeof(struct ::sockaddr_in));
}

std::string
address::addrstr () const
{
    char buf[INET6_ADDRSTRLEN];
    const char *ret =
        ::inet_ntop(inet6() ? AF_INET6 : AF_INET,
                    inet6() ? (void *)&m_sa.v6.sin6_addr : (void *)&m_sa.v4.sin_addr,
                    buf, sizeof(buf)
                    );
    if (!ret)
        throw k::system_error("::inet_ntop() error");
    return std::string(buf);
}

uint16_t
address::port () const noexcept
{
    return ::ntohs(inet6() ? m_sa.v4.sin_port : m_sa.v6.sin6_port);
}

bool
address::is_ipv4_mapped () const noexcept
{
    assert(inet6());
    return IN6_IS_ADDR_V4MAPPED(&m_sa.v6.sin6_addr);
}

bool
address::is_valid_inet_addrstr (const std::string &addrstr)
{
    struct ::sockaddr_in sa;
    int ret = ::inet_pton(AF_INET, addrstr.data(), &sa);
    if (ret < 0)
        throw k::system_error("::inet_pton() error");
    return ret;
}

bool
address::is_valid_inet6_addrstr (const std::string &addrstr)
{
    struct ::sockaddr_in sa;
    int ret = ::inet_pton(AF_INET6, addrstr.data(), &sa);
    if (ret < 0)
        throw k::system_error("::inet_pton() error");
    return ret;
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

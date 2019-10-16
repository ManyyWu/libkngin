#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif
#include <cstdio>
#include <cstring>
#include "define.h"
#include "filefd.h"
#include "sockopts.h"
#include "logfile.h"
#include "common.h"

__NAMESPACE_BEGIN

sockopts::__sockopts_info sockopts::m_opts_entry[SOCKOPTS_TYPE_MAX] = {
    { "SO_BROADCAST",      SOL_SOCKET,   SO_BROADCAST,      sockopts::get_flag,    sockopts::set_flag    },
    { "SO_DEBUG",          SOL_SOCKET,   SO_DEBUG,          sockopts::get_flag,    sockopts::set_flag    },
    { "SO_DONTROUTE",      SOL_SOCKET,   SO_DONTROUTE,      sockopts::get_flag,    sockopts::set_flag    },
    { "SO_ERROR",          SOL_SOCKET,   SO_ERROR,          sockopts::get_int,     NULL                  },
    { "SO_KEEPALIVE",      SOL_SOCKET,   SO_KEEPALIVE,      sockopts::get_flag,    sockopts::set_flag    },
    { "SO_LINGER",         SOL_SOCKET,   SO_LINGER,         sockopts::get_linger,  sockopts::set_linger  },
    { "SO_OOBINLINE",      SOL_SOCKET,   SO_OOBINLINE,      sockopts::get_flag,    sockopts::set_flag    },
    { "SO_RCVBUF",         SOL_SOCKET,   SO_RCVBUF,         sockopts::get_int,     sockopts::set_int     },
    { "SO_SNDBUF",         SOL_SOCKET,   SO_SNDBUF,         sockopts::get_int,     sockopts::set_int     },
    { "SO_RCVLOWAT",       SOL_SOCKET,   SO_RCVLOWAT,       sockopts::get_int,     sockopts::set_int     },
    { "SO_SNDLOWAT",       SOL_SOCKET,   SO_SNDLOWAT,       sockopts::get_int,     sockopts::set_int     },
    { "SO_RCVTIMEO",       SOL_SOCKET,   SO_RCVTIMEO,       sockopts::get_timeval, sockopts::set_timeval },
    { "SO_SNDTIMEO",       SOL_SOCKET,   SO_SNDTIMEO,       sockopts::get_timeval, sockopts::set_timeval },
    { "SO_REUSEADDR",      SOL_SOCKET,   SO_REUSEADDR,      sockopts::get_flag,    sockopts::set_flag    },
#ifdef SO_REUSEPORT
    { "SO_REUSEPORT",      SOL_SOCKET,   SO_REUSEPORT,      sockopts::get_flag,    sockopts::set_flag    },
#else
    { "SO_REUSEPORT",      0,            0,                 NULL,                  NULL                  },
#endif
    { "SO_TYPE",           SOL_SOCKET,   SO_TYPE,           sockopts::get_int,     NULL                  },
    { "IP_TOS",            IPPROTO_IP,   IP_TOS,            sockopts::get_int,     sockopts::set_int     },
    { "IP_TTL",            IPPROTO_IP,   IP_TTL,            sockopts::get_int,     sockopts::set_int     },
#ifdef IPV6_UNICAST_HOPS
    { "IPV6_UNICAST_HOPS", IPPROTO_IPV6, IPV6_UNICAST_HOPS, sockopts::get_int,     sockopts::set_int     },
#else
    { "IPV6_UNICAST_HOPS", 0,            0,                 NULL,                  NULL                  },
#endif
#ifdef IPV6_V6ONLY
    { "IPV6_V6ONLY",       IPPROTO_IPV6, IPV6_V6ONLY,       sockopts::get_flag,    sockopts::set_flag    },
#else
    { "IPV6_V6ONLY",       0,            0,                 NULL,                  NULL                  },
#endif
    { "TCP_MAXSEG",        IPPROTO_TCP,  TCP_MAXSEG,        sockopts::get_int,     sockopts::set_int     },
    { "TCP_NODELAY",       IPPROTO_TCP,  TCP_NODELAY,       sockopts::get_flag,    sockopts::set_flag    },
};

sockopts::sockopts (int _fd)
    : m_fd(_fd)
{
    kassert(__fd_valid(_fd));
}

bool
sockopts::get_flag (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen;
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.i_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(int));
    return (_ret >= 0);
}

bool
sockopts::get_int (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen;
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.l_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(long));
    return (_ret >= 0);
}

bool
sockopts::get_linger (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen;
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.linger_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(struct linger));
    return (_ret >= 0);
}

bool
sockopts::get_timeval (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen;
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.timeval_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(struct timeval));
    return (_ret >= 0);
}

bool
sockopts::set_flag (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.i_val, sizeof(_val.i_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_int (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.l_val, sizeof(_val.l_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_linger (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.linger_val, sizeof(_val.linger_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_timeval (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.timeval_val, sizeof(_val.timeval_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

__NAMESPACE_END

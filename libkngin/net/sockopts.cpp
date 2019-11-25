#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif
#include "core/common.h"
#include "net/filefd.h"
#include "net/sockopts.h"
#include "net/socket.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/sockopts.cpp"

KNGIN_NAMESPACE_K_BEGIN

const sockopts::sockopts_info sockopts::opts_entry[SOCKOPTS_TYPE_MAX] = {
    { "SO_BROADCAST", SOL_SOCKET,   SO_BROADCAST },
    { "SO_DEBUG",     SOL_SOCKET,   SO_DEBUG     },
    { "SO_DONTROUTE", SOL_SOCKET,   SO_DONTROUTE },
    { "SO_ERROR",     SOL_SOCKET,   SO_ERROR     },
    { "SO_KEEPALIVE", SOL_SOCKET,   SO_KEEPALIVE },
    { "SO_LINGER",    SOL_SOCKET,   SO_LINGER    },
    { "SO_OOBINLINE", SOL_SOCKET,   SO_OOBINLINE },
    { "SO_RCVBUF",    SOL_SOCKET,   SO_RCVBUF    },
    { "SO_SNDBUF",    SOL_SOCKET,   SO_SNDBUF    },
    { "SO_RCVLOWAT",  SOL_SOCKET,   SO_RCVLOWAT  },
    { "SO_SNDLOWAT",  SOL_SOCKET,   SO_SNDLOWAT  },
    { "SO_RCVTIMEO",  SOL_SOCKET,   SO_RCVTIMEO  },
    { "SO_SNDTIMEO",  SOL_SOCKET,   SO_SNDTIMEO  },
    { "SO_REUSEADDR", SOL_SOCKET,   SO_REUSEADDR },
#ifdef SO_REUSEPORT
    { "SO_REUSEPORT", SOL_SOCKET,   SO_REUSEPORT },
#else
    { "SO_REUSEPORT", 0,            0            },
#endif
    { "SO_TYPE",      SOL_SOCKET,   SO_TYPE      },
    { "IP_TOS",       IPPROTO_IP,   IP_TOS       },
    { "IP_TTL",       IPPROTO_IP,   IP_TTL       },
#ifdef IPV6_V6ONLY
    { "IPV6_V6ONLY",  IPPROTO_IPV6, IPV6_V6ONLY  },
#else
    { "IPV6_V6ONLY",  0,            0            },
#endif
    { "TCP_MAXSEG",   IPPROTO_TCP,  TCP_MAXSEG   },
    { "TCP_NODELAY",  IPPROTO_TCP,  TCP_NODELAY  },
};

bool
sockopts::get_flag (int _fd, const sockopts_info &_opt_info, bool &_flag)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(int);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_value.i_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    check(_optlen == sizeof(int));
    _flag = _value.i_val;
    return (_ret >= 0);
}

bool
sockopts::get_int (int _fd, const sockopts_info &_opt_info, int &_val)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(int);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_value.i_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    check(_optlen == sizeof(int));
    _val = _value.i_val;
    return (_ret >= 0);
}

bool
sockopts::get_linger (int _fd, const sockopts_info &_opt_info, struct linger &_l)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(struct linger);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_value.linger_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    check(_optlen == sizeof(struct linger));
    return (_ret >= 0);
}

bool
sockopts::get_timeval (int _fd, const sockopts_info &_opt_info, struct timeval &_tv)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(timeval);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_value.timeval_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    check(_optlen == sizeof(struct timeval));
    _tv = _value.timeval_val;
    return (_ret >= 0);
}

bool
sockopts::set_flag (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info)
{
    return set_int(_val, _fd, _opt_info);
}

bool
sockopts::set_int (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_val.i_val, sizeof(_val.i_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_linger (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_val.linger_val, sizeof(_val.linger_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_timeval (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_val.timeval_val, sizeof(_val.timeval_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

KNGIN_NAMESPACE_K_END

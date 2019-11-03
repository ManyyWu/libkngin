#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif
#include <cstdio>
#include "define.h"
#include "error.h"
#include "filefd.h"
#include "sockopts.h"
#include "socket.h"
#include "logfile.h"
#include "common.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/sockopts.cpp"

__NAMESPACE_BEGIN

const sockopts::__sockopts_info sockopts::opts_entry[SOCKOPTS_TYPE_MAX] = {
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
sockopts::get_flag (int _fd, const __sockopts_info &_opt_info, bool &_flag)
{
    __sockopt_val _value;
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
sockopts::get_int (int _fd, const __sockopts_info &_opt_info, int &_val)
{
    __sockopt_val _value;
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
sockopts::get_linger (int _fd, const __sockopts_info &_opt_info, struct linger &_l)
{
    __sockopt_val _value;
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
sockopts::get_timeval (int _fd, const __sockopts_info &_opt_info, struct timeval &_tv)
{
    __sockopt_val _value;
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
sockopts::set_flag (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    return set_int(_val, _fd, _opt_info);
}

bool
sockopts::set_int (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_val.i_val, sizeof(_val.i_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_linger (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_val.linger_val, sizeof(_val.linger_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_timeval (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                            &_val.timeval_val, sizeof(_val.timeval_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d",
                  _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

__NAMESPACE_END

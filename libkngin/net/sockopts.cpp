#ifndef _WIN32
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif
#include "core/base/system_error.h"
#include "core/base/timestamp.h"
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
#ifndef _WIN32
    { "TCP_INFO",     IPPROTO_TCP,  TCP_INFO     },
#endif
};

bool
sockopts::get_flag (socket_type _fd, const sockopts_info &_opt_info)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(int);
    if (::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     (char *)&_value.i_val, &_optlen) < 0) {
        throw k::system_error(std::string("::getsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
    return _value.i_val;
}

int
sockopts::get_int (socket_type _fd, const sockopts_info &_opt_info)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(int);
    if (::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     (char *)&_value.i_val, &_optlen) < 0) {
        throw k::system_error(std::string("::getsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
    return _value.i_val;
}

struct ::linger
sockopts::get_linger (socket_type _fd, const sockopts_info &_opt_info)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(struct ::linger);
    if (::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     (char *)&_value.linger_val, &_optlen) < 0) {
        throw k::system_error(std::string("::getsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
    return _value.linger_val;
}

struct ::timeval
sockopts::get_timeval (socket_type _fd, const sockopts_info &_opt_info)
{
#ifdef _WIN32
    struct ::timeval _value;
    timestamp(get_int(_fd, _opt_info)).to_timeval(_value);
    return _value;
#else
    sockopt_val _value;
    socklen_t _optlen = sizeof(struct ::timeval);
    if (::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     (char *)&_value.timeval_val, &_optlen) < 0) {
        throw k::system_error(std::string("::getsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
    return _value.timeval_val;
#endif
}

#ifndef _WIN32
struct ::tcp_info
sockopts::get_tcp_info (socket_type _fd, const sockopts_info &_opt_info)
{
    sockopt_val _value;
    socklen_t _optlen = sizeof(struct ::tcp_info);
    if (::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     &_value.tcp_info, &_optlen) < 0) {
        throw k::system_error(std::string("::getsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
    assert(_optlen == sizeof(struct ::tcp_info));
    return _value.tcp_info;
}
#endif

void
sockopts::set_flag (const sockopt_val &_val, socket_type _fd, 
                    const sockopts_info &_opt_info)
{
    set_int(_val, _fd, _opt_info);
}

void
sockopts::set_int (const sockopt_val &_val, socket_type _fd, 
                   const sockopts_info &_opt_info)
{
    if (::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     (const char *)&_val.i_val, sizeof(_val.i_val)) < 0) {
        throw k::system_error(std::string("::setsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
}

void
sockopts::set_linger (const sockopt_val &_val, socket_type _fd, 
                      const sockopts_info &_opt_info)
{
    if (::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     (const char *)&_val.linger_val, sizeof(_val.linger_val)) < 0) {
        throw k::system_error(std::string("::setsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
}

void
sockopts::set_timeval (const sockopt_val &_val, socket_type _fd, 
                       const sockopts_info &_opt_info)
{
#ifdef _WIN32
    sockopt_val _temp_val; 
    _temp_val.i_val = timestamp(_val.timeval_val).value_int();
    set_int(_temp_val, _fd, _opt_info);
#else
    if (::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name,
                     (const char *)&_val.timeval_val, sizeof(_val.timeval_val)) < 0) {
        throw k::system_error(std::string("::setsockopt() error, option = ")
                              + _opt_info.opt_str);
    }
#endif
}

KNGIN_NAMESPACE_K_END

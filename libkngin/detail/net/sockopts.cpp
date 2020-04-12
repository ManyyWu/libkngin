#include "kngin/net/sockopts.h"
#if defined(KNGIN_SYSTEM_WIN32)
#else
#  include <netinet/tcp.h>
#  include <netinet/ip.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/sockopts.cpp"

namespace k {

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
#if !defined(KNGIN_SYSTEM__WIN32)
    { "TCP_INFO",     IPPROTO_TCP,  TCP_INFO     },
#endif /* defined(KNGIN_SYSTEM__WIN32) */
};

#define throw_getsockopts_error()                                      \
    throw_system_error((std::string("::getsockopt() error, option = ") \
                           + opt_info.opt_str).c_str(), last_error())

#define throw_setsockopts_error()                                      \
    throw_system_error((std::string("::setsockopt() error, option = ") \
                           + opt_info.opt_str).c_str(), last_error())

void
sockopts::set_flag (const sockopt_val &val, handle_t h,
                    const sockopts_info &opt_info)
{
  set_int(val, h, opt_info);
}

void
sockopts::set_int (const sockopt_val &val, handle_t h,
                   const sockopts_info &opt_info)
{
  if (::setsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   (const char *)&val.i_val, sizeof(val.i_val)) < 0)
    throw_setsockopts_error();
}

void
sockopts::set_linger (const sockopt_val &val, handle_t h,
                      const sockopts_info &opt_info)
{
  if (::setsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   (const char *)&val.linger_val, sizeof(val.linger_val)) < 0)
    throw_setsockopts_error();
}

void
sockopts::set_timeval (const sockopt_val &val, handle_t h,
                       const sockopts_info &opt_info)
{
#if defined(KNGIN_SYSTEM__WIN32)
  sockopt_val temp_val;
    temp_val.i_val = timestamp(val.timeval_val).value_int();
    set_int(temp_val, h, opt_info);
#else
  if (::setsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   (const char *)&val.timeval_val, sizeof(val.timeval_val)) < 0)
    throw_setsockopts_error();
#endif /* defined(KNGIN_SYSTEM__WIN32) */
}

bool
sockopts::get_flag (handle_t h, const sockopts_info &opt_info)
{
  sockopt_val value;
  socklen_t optlen = sizeof(int);
  if (::getsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   (char *)&value.i_val, &optlen) < 0)
    throw_getsockopts_error();
  return value.i_val;
}

int
sockopts::get_int (handle_t h, const sockopts_info &opt_info)
{
  sockopt_val value;
  socklen_t optlen = sizeof(int);
  if (::getsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   (char *)&value.i_val, &optlen) < 0)
    throw_getsockopts_error();
  return value.i_val;
}

struct ::linger
sockopts::get_linger (handle_t h, const sockopts_info &opt_info)
{
  sockopt_val value;
  socklen_t optlen = sizeof(struct ::linger);
  if (::getsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   (char *)&value.linger_val, &optlen) < 0)
    throw_getsockopts_error();
  return value.linger_val;
}

struct ::timeval
sockopts::get_timeval (handle_t h, const sockopts_info &opt_info)
{
#if defined(KNGIN_SYSTEM__WIN32)
  struct ::timeval value;
  timestamp(get_int(h, opt_info)).to_timeval(value);
  return value;
#else
  sockopt_val value;
  socklen_t optlen = sizeof(struct ::timeval);
  if (::getsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   (char *)&value.timeval_val, &optlen) < 0)
    throw_getsockopts_error();
  return value.timeval_val;
#endif /* defined(KNGIN_SYSTEM__WIN32) */
}

#if defined(KNGIN_SYSTEM__WIN32)
struct ::tcp_info
sockopts::get_tcp_info (handle_t h, const sockopts_info &opt_info)
{
  sockopt_val value;
  socklen_t optlen = sizeof(struct ::tcp_info);
  if (::getsockopt(h, opt_info.opt_level, opt_info.opt_name,
                   &value.tcp_info, &optlen) < 0)
    throw_getsockopts_error();
  assert(optlen == sizeof(struct ::tcp_info));
  return value.tcp_info;
}
#endif /* defined(KNGIN_SYSTEM__WIN32) */

} /* namespace k */
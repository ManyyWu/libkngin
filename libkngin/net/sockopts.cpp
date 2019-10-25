#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif
#include <cstdio>
#include <cstring>
#include "define.h"
#include "error.h"
#include "filefd.h"
#include "sockopts.h"
#include "socket.h"
#include "logfile.h"
#include "common.h"

__NAMESPACE_BEGIN

const sockopts::__sockopts_info sockopts::opts_entry[SOCKOPTS_TYPE_MAX] = {
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
#ifdef IPV6_V6ONLY
    { "IPV6_V6ONLY",       IPPROTO_IPV6, IPV6_V6ONLY,       sockopts::get_flag,    sockopts::set_flag    },
#else
    { "IPV6_V6ONLY",       0,            0,                 NULL,                  NULL                  },
#endif
    { "TCP_MAXSEG",        IPPROTO_TCP,  TCP_MAXSEG,        sockopts::get_int,     sockopts::set_int     },
    { "TCP_NODELAY",       IPPROTO_TCP,  TCP_NODELAY,       sockopts::get_flag,    sockopts::set_flag    },
};

retf_bool
sockopts::broadcast (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_broadcast (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]);
    return {(bool)!_ret};
}

retf_bool
sockopts::debug (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_debug (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]);
    return {(bool)!_ret};
}

retf_bool
sockopts::nonroute (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_nonroute (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]);
    return {(bool)!_ret};
}

retf_int32
sockopts::error (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_ERROR]);
    return {(bool)!_ret, _val.i_val};
}

retf_bool
sockopts::keepalive (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_keepalive (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]);
    return {(bool)!_ret};
}

retf_void
sockopts::linger (socket &_s, struct linger &_l)
{
    __sockopt_val _val;
    int _ret = sockopts::get_linger(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]);
    _l = _val.linger_val;
    return {(bool)!_ret};
}

retf_void
sockopts::set_linger (socket &_s, bool _on, int _t /* = 0 */)
{
    __sockopt_val _val;
    _val.linger_val = {(int)_on, _t};
    int _ret = sockopts::set_linger(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]);
    return {(bool)!_ret};
}

retf_bool
sockopts::oobinline (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_ooblinline (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]);
    return {(bool)!_ret};
}

retf_int32
sockopts::rcvbuf (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]);
    return {(bool)!_ret, (int)_val.i_val};
}

retf_void
sockopts::set_rcvbuf (socket &_s, int _size)
{
    __sockopt_val _val{_size};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]);
    return {(bool)!_ret};
}

retf_int32
sockopts::sndbuf (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]);
    return {(bool)!_ret, (int)_val.i_val};
}

retf_void
sockopts::set_sndbuf (socket &_s, int _size)
{
    __sockopt_val _val{_size};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]);
    return {(bool)!_ret};
}

retf_int32
sockopts::rcvlowat (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]);
    return {(bool)!_ret, (int)_val.i_val};
}

retf_void
sockopts::set_rcvlowat (socket &_s, int _size)
{
    __sockopt_val _val{_size};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]);
    return {(bool)!_ret};
}

retf_int32
sockopts::sndlowat (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]);
    return {(bool)!_ret, (int)_val.i_val};
}

retf_void
sockopts::set_sndlowat (socket &_s, int _size)
{
    __sockopt_val _val{_size};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]);
    return {(bool)!_ret};
}

retf_void
sockopts::rcvtimeo (socket &_s, struct timeval &_t)
{
    __sockopt_val _val;
    int _ret = sockopts::get_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]);
    _val.timeval_val = _t;
    return {(bool)!_ret};
}

retf_void
sockopts::set_rcvtimeo (socket &_s, struct timeval _t)
{
    __sockopt_val _val;
    _val.timeval_val = _t;
    int _ret = sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]);
    return {(bool)!_ret};
}

retf_void
sockopts::sndtimeo (socket &_s, struct timeval &_t)
{
    __sockopt_val _val;
    int _ret = sockopts::get_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]);
    _t = _val.timeval_val;
    return {(bool)!_ret};
}

retf_void
sockopts::set_sndtimeo (socket &_s, struct timeval _t)
{
    __sockopt_val _val;
    _val.timeval_val = _t;
    int _ret = sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]);
    return {(bool)!_ret};
}

retf_bool
sockopts::reuseaddr (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_reuseaddr (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]);
    return {(bool)!_ret};
}

retf_bool
sockopts::reuseport (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_reuseport (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]);
    return {(bool)!_ret};
}

retf_int32
sockopts::type (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TYPE]);
    return {(bool)!_ret, _val.i_val};
}

retf_int32
sockopts::ip_tos (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]);
    return {(bool)!_ret, _val.i_val};
}

retf_void
sockopts::set_ip_tos (socket &_s, int _t)
{
    __sockopt_val _val{_t};
    int _ret = sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]);
    return {(bool)!_ret};
}

retf_int32
sockopts::ip_ttl (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]);
    return {(bool)!_ret, _val.i_val};
}

retf_void
sockopts::set_ip_ttl (socket &_s, int _t)
{
    __sockopt_val _val{_t};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]);
    return {(bool)!_ret};
}

retf_bool
sockopts::ipv4_disabled  (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_ipv6_only (socket &_s, bool _on)
{
    __sockopt_val _val{_on};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]);
    return {(bool)!_ret};
}

retf_int32
sockopts::maxseg (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]);
    return {(bool)!_ret, _val.i_val};
}

retf_void
sockopts::set_maxseg (socket &_s, int _size)
{
    __sockopt_val _val{_size};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]);
    return {(bool)!_ret};
}

retf_bool
sockopts::nodelay (socket &_s)
{
    __sockopt_val _val{0};
    int _ret = sockopts::get_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]);
    return {(bool)!_ret, (bool)_val.i_val};
}

retf_void
sockopts::set_nodelay (socket &_s, int _size)
{
    __sockopt_val _val{_size};
    int _ret = sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]);
    return {(bool)!_ret};
}

bool
sockopts::get_flag (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen = sizeof(int);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.i_val, &_optlen);
    if (_ret < 0)
                log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(int));
    return (_ret >= 0);
}

bool
sockopts::get_int (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen = sizeof(int);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.i_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(int));
    return (_ret >= 0);
}

bool
sockopts::get_linger (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen = sizeof(struct linger);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.linger_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(struct linger));
    return (_ret >= 0);
}

bool
sockopts::get_timeval (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    socklen_t _optlen = sizeof(timeval);
    int _ret = ::getsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.timeval_val, &_optlen);
    if (_ret < 0)
        log_error("::getsockopt() get option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    kassert(_optlen == sizeof(struct timeval));
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
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.i_val, sizeof(_val.i_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_linger (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.linger_val, sizeof(_val.linger_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

bool
sockopts::set_timeval (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info)
{
    int _ret = ::setsockopt(_fd, _opt_info.opt_level, _opt_info.opt_name, &_val.timeval_val, sizeof(_val.timeval_val));
    if (_ret < 0)
        log_error("::setsockopt() set option %s failed - %s:%d", _opt_info.opt_str, strerror(errno), errno);
    return (_ret >= 0);
}

__NAMESPACE_END

#ifndef KNGIN_SOCK_OPTS_H
#define KNGIN_SOCK_OPTS_H

#include <netinet/tcp.h>
#include <time.h>
#include "core/define.h"
#include "core/error.h"
#include "core/system_error.h"
#include "net/socket.h"

KNGIN_NAMESPACE_K_BEGIN

class sockopts {
public:
protected:
    union sockopt_val {
        int               i_val;
        long              l_val;
        struct ::linger   linger_val;
        struct ::timeval  timeval_val;
        struct ::tcp_info tcp_info;
    };

    struct sockopts_info {
        const char *opt_str;
        int         opt_level;
        int         opt_name;
    };

    enum SOCKOPTS_TYPE {
        SOCKOPTS_TYPE_BROADCAST = 0,
        SOCKOPTS_TYPE_DEBUG,          // it needs to be set as root
        SOCKOPTS_TYPE_DONTROUTE,
        SOCKOPTS_TYPE_ERROR,
        SOCKOPTS_TYPE_KEEPALIVE,
        SOCKOPTS_TYPE_LINGER,
        SOCKOPTS_TYPE_OOBINLINE,
        SOCKOPTS_TYPE_RCVBUF,
        SOCKOPTS_TYPE_SNDBUF,
        SOCKOPTS_TYPE_RCVLOWAT,
        SOCKOPTS_TYPE_SNDLOWAT,
        SOCKOPTS_TYPE_RCVTIMEO,
        SOCKOPTS_TYPE_SNDTIMEO,
        SOCKOPTS_TYPE_REUSEADDR,
        SOCKOPTS_TYPE_REUSEPORT,
        SOCKOPTS_TYPE_TYPE,
        SOCKOPTS_TYPE_IP_TOS,
        SOCKOPTS_TYPE_IP_TTL,
        SOCKOPTS_TYPE_IPV6_V6ONLY,
        SOCKOPTS_TYPE_TCP_MAXSEG,
        SOCKOPTS_TYPE_TCP_NODELAY,
        SOCKOPTS_TYPE_TCP_INFO,
        SOCKOPTS_TYPE_MAX
    };

private:
    sockopts       () = default;

    ~sockopts      () = default;

public:
    static bool
    broadcast      (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]); }
    static void
    set_broadcast  (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]); }
    static bool
    debug          (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]); }
    static void
    set_debug      (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]); }
    static bool
    nonroute       (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]); }
    static void
    set_nonroute   (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]); }
    static std::error_code
    error          (const socket &_s)
    { return int2ec(sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_ERROR])); }
    static bool
    keepalive      (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]); }
    static void
    set_keepalive  (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]); }
    static struct ::linger
    linger         (const socket &_s)
    { return sockopts::get_linger(_s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]); }
    static void
    set_linger     (const socket &_s, bool _on, int _t /* = 0 */)
    { sockopt_val _val; _val.linger_val = {static_cast<int>(_on), _t}; sockopts::set_linger(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]); }
    static bool
    oobinline      (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]); }
    static void
    set_ooblinline (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]); }
    static int
    rcvbuf         (const socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]); }
    static void
    set_rcvbuf     (const socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]); }
    static int
    sndbuf         (const socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]); }
    static void
    set_sndbuf     (const socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]); }
    static int
    rcvlowat       (const socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]); }
    static void
    set_rcvlowat   (const socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]); }
    static int
    sndlowat       (const socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]); }
    static void
    set_sndlowat   (const socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]); }
    static struct ::timeval
    rcvtimeo       (const socket &_s)
    { return sockopts::get_timeval(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]); }
    static void
    set_rcvtimeo   (const socket &_s, struct ::timeval _t)
    { sockopt_val _val; _val.timeval_val = _t; sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]); }
    static struct ::timeval
    sndtimeo       (const socket &_s)
    { return sockopts::get_timeval(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]); }
    static void
    set_sndtimeo   (const socket &_s, struct ::timeval _t)
    { sockopt_val _val; _val.timeval_val = _t; sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]); }
    static bool
    reuseaddr      (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]); }
    static void
    set_reuseaddr  (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]); }
    static bool
    reuseport      (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]); }
    static void
    set_reuseport  (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]); }
    static int
    type           (const socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_TYPE]); }
    static int
    ip_tos         (const socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]); }
    static void
    set_ip_tos     (const socket &_s, int _t)
    { sockopt_val _val{_t}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]); }
    static int
    ip_ttl         (const socket &_s, int &_ttl)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]); }
    static void
    set_ip_ttl     (const socket &_s, int _t)
    { sockopt_val _val{_t}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]); }
    static bool
    ipv4_disabled  (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]); }
    static void
    set_ipv6_only  (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]); }
    static int
    maxseg         (const socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]); }
    static void
    set_maxseg     (const socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]); }
    static bool
    nodelay        (const socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]); }
    static void
    set_nodelay    (const socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]); }
    static struct ::tcp_info
    tcp_info       (const socket &_s)
    { return sockopts::get_tcp_info(_s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_INFO]); }

protected:
    static bool
    get_flag       (int _fd, const sockopts_info &_opt_info);

    static int
    get_int        (int _fd, const sockopts_info &_opt_info);

    static struct ::linger
    get_linger     (int _fd, const sockopts_info &_opt_info);

    static struct ::timeval
    get_timeval    (int _fd, const sockopts_info &_opt_info);

    static struct ::tcp_info
    get_tcp_info   (int _fd, const sockopts_info &_opt_info);

    static void
    set_flag       (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

    static void
    set_int        (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

    static void
    set_linger     (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

    static void
    set_timeval    (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

protected:
    static const sockopts_info opts_entry[SOCKOPTS_TYPE_MAX];
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SOCK_OPTS_H */
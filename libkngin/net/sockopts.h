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
        int            i_val;
        long           l_val;
        struct linger  linger_val;
        struct timeval timeval_val;
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
        SOCKOPTS_TYPE_MAX
    };

private:
    sockopts       () = default;

    ~sockopts      () = default;

public:
    static bool
    broadcast      (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]); }
    static void
    set_broadcast  (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]); }
    static bool
    debug          (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]); }
    static void
    set_debug      (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]); }
    static bool
    nonroute       (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]); }
    static void
    set_nonroute   (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]); }
    static std::error_code
    error          (socket &_s)
    { return int2ec(sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_ERROR])); }
    static bool
    keepalive      (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]); }
    static void
    set_keepalive  (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]); }
    static struct linger
    linger         (socket &_s)
    { return sockopts::get_linger(_s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]); }
    static void
    set_linger     (socket &_s, bool _on, int _t /* = 0 */)
    { sockopt_val _val; _val.linger_val = {static_cast<int>(_on), _t}; sockopts::set_linger(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]); }
    static bool
    oobinline      (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]); }
    static void
    set_ooblinline (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]); }
    static int
    rcvbuf         (socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]); }
    static void
    set_rcvbuf     (socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]); }
    static int
    sndbuf         (socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]); }
    static void
    set_sndbuf     (socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]); }
    static int
    rcvlowat       (socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]); }
    static void
    set_rcvlowat   (socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]); }
    static int
    sndlowat       (socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]); }
    static void
    set_sndlowat   (socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]); }
    static struct timeval
    rcvtimeo       (socket &_s)
    { return sockopts::get_timeval(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]); }
    static void
    set_rcvtimeo   (socket &_s, struct timeval _t)
    { sockopt_val _val; _val.timeval_val = _t; sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]); }
    static struct timeval
    sndtimeo       (socket &_s)
    { return sockopts::get_timeval(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]); }
    static void
    set_sndtimeo   (socket &_s, struct timeval _t)
    { sockopt_val _val; _val.timeval_val = _t; sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]); }
    static bool
    reuseaddr      (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]); }
    static void
    set_reuseaddr  (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]); }
    static bool
    reuseport      (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]); }
    static void
    set_reuseport  (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]); }
    static int
    type           (socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_TYPE]); }
    static int
    ip_tos         (socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]); }
    static void
    set_ip_tos     (socket &_s, int _t)
    { sockopt_val _val{_t}; sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]); }
    static int
    ip_ttl         (socket &_s, int &_ttl)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]); }
    static void
    set_ip_ttl     (socket &_s, int _t)
    { sockopt_val _val{_t}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]); }
    static bool
    ipv4_disabled  (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]); }
    static void
    set_ipv6_only  (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]); }
    static int
    maxseg         (socket &_s)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]); }
    static void
    set_maxseg     (socket &_s, int _size)
    { sockopt_val _val{_size}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]); }
    static bool
    nodelay        (socket &_s)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]); }
    static void
    set_nodelay    (socket &_s, bool _on)
    { sockopt_val _val{_on}; sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]); }

protected:
    static bool
    get_flag       (int _fd, const sockopts_info &_opt_info);

    static int
    get_int        (int _fd, const sockopts_info &_opt_info);

    static struct linger
    get_linger     (int _fd, const sockopts_info &_opt_info);

    static struct timeval
    get_timeval    (int _fd, const sockopts_info &_opt_info);

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
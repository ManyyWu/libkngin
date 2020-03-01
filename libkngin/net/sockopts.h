#ifndef KNGIN_SOCK_OPTS_H
#define KNGIN_SOCK_OPTS_H

#ifdef _WIN32
#else
#include <netinet/tcp.h>
#endif
#include <time.h>
#include "core/base/define.h"
#include "core/base/system_error.h"
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
#ifndef _WIN32
        struct ::tcp_info tcp_info;
#endif
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
#ifndef _WIN32
        SOCKOPTS_TYPE_TCP_INFO,
#endif
        SOCKOPTS_TYPE_MAX
    };

private:
    sockopts       () = default;

    ~sockopts      () = default;

public:
    static bool
    broadcast      (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]); }
    static void
    set_broadcast  (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]); }
    static bool
    debug          (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]); }
    static void
    set_debug      (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]); }
    static bool
    nonroute       (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]); }
    static void
    set_nonroute   (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]); }
    static error_code
    error          (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_ERROR]); }
    static bool
    keepalive      (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]); }
    static void
    set_keepalive  (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]); }
    static struct ::linger
    linger         (const socket &s)
    { return sockopts::get_linger(s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]); }
    static void
#ifdef _WIN32
    set_linger     (const socket &s, bool on, u_short t /* = 0 */)
#else
    set_linger     (const socket &s, bool on, int t /* = 0 */)
#endif
    { sockopt_val val; val.linger_val = {on, t}; sockopts::set_linger(val, s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]); }
    static bool
    oobinline      (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]); }
    static void
    set_ooblinline (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]); }
    static int
    rcvbuf         (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]); }
    static void
    set_rcvbuf     (const socket &s, int size)
    { sockopt_val val{size}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]); }
    static int
    sndbuf         (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]); }
    static void
    set_sndbuf     (const socket &s, int size)
    { sockopt_val val{size}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]); }
    static int
    rcvlowat       (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]); }
    static void
    set_rcvlowat   (const socket &s, int size)
    { sockopt_val val{size}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]); }
    static int
    sndlowat       (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]); }
    static void
    set_sndlowat   (const socket &s, int size)
    { sockopt_val val{size}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]); }
    static struct ::timeval
    rcvtimeo       (const socket &s)
    { return sockopts::get_timeval(s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]); }
    static void
    set_rcvtimeo   (const socket &s, struct ::timeval t)
    { sockopt_val val; val.timeval_val = t; sockopts::set_timeval(val, s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]); }
    static struct ::timeval
    sndtimeo       (const socket &s)
    { return sockopts::get_timeval(s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]); }
    static void
    set_sndtimeo   (const socket &s, struct ::timeval t)
    { sockopt_val val; val.timeval_val = t; sockopts::set_timeval(val, s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]); }
    static bool
    reuseaddr      (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]); }
    static void
    set_reuseaddr  (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]); }
    static bool
    reuseport      (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]); }
    static void
    set_reuseport  (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]); }
    static int
    type           (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_TYPE]); }
    static int
    ip_tos         (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]); }
    static void
    set_ip_tos     (const socket &s, int t)
    { sockopt_val val{t}; sockopts::set_flag(val, s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]); }
    static int
    ip_ttl         (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]); }
    static void
    set_ip_ttl     (const socket &s, int t)
    { sockopt_val val{t}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]); }
    static bool
    ipv4_disabled  (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]); }
    static void
    set_ipv6_only  (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]); }
    static int
    maxseg         (const socket &s)
    { return sockopts::get_int(s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]); }
    static void
    set_maxseg     (const socket &s, int size)
    { sockopt_val val{size}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]); }
    static bool
    nodelay        (const socket &s)
    { return sockopts::get_flag(s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]); }
    static void
    set_nodelay    (const socket &s, bool on)
    { sockopt_val val{on}; sockopts::set_int(val, s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]); }
//    static struct ::tcp_info
//    tcp_info       (const socket &s)
//    { return sockopts::get_tcp_info(s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_INFO]); }

protected:
    static bool
    get_flag       (socket_type fd, const sockopts_info &opt_info);

    static int
    get_int        (socket_type fd, const sockopts_info &opt_info);

    static struct ::linger
    get_linger     (socket_type fd, const sockopts_info &opt_info);

    static struct ::timeval
    get_timeval    (socket_type fd, const sockopts_info &opt_info);

#ifndef _WIN32
    static struct ::tcp_info
    get_tcp_info   (int fd, const sockopts_info &opt_info);
#endif

    static void
    set_flag       (const sockopt_val &val, socket_type fd, const sockopts_info &opt_info);

    static void
    set_int        (const sockopt_val &val, socket_type fd, const sockopts_info &opt_info);

    static void
    set_linger     (const sockopt_val &val, socket_type fd, const sockopts_info &opt_info);

    static void
    set_timeval    (const sockopt_val &val, socket_type fd, const sockopts_info &opt_info);

protected:
    static const sockopts_info opts_entry[SOCKOPTS_TYPE_MAX];
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SOCK_OPTS_H */
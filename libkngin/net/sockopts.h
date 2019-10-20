#ifndef _SOCK_OPTS_H_
#define _SOCK_OPTS_H_

#include <netinet/tcp.h>
#include "define.h"
#include "error.h"

__NAMESPACE_BEGIN

class sockopts {
public:
protected:
    union __sockopt_val {
        int            i_val;
        long           l_val;
        struct linger  linger_val;
        struct timeval timeval_val;
    };

    struct __sockopts_info {
        const char *opt_str;
        int         opt_level;
        int         opt_name;
        bool        (*opt_get)(__sockopt_val &, int, const __sockopts_info &);
        bool        (*opt_set)(const __sockopt_val &, int, const __sockopts_info &);
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

public:
    sockopts       (int _fd);

public:
    retf_bool
    broadcast      ();

    retf_void
    set_broadcast  (bool _on);

    retf_bool
    debug          ();

    retf_void
    set_debug      (bool _on);

    retf_bool
    nonroute       ();

    retf_void
    set_nonroute   (bool _on);

    retf_int32
    error          ();

    retf_bool
    keepalive      ();

    retf_void
    set_keepalive  (bool _on);

    retf_void
    linger         (struct linger &_l);

    retf_void
    set_linger     (bool _on, int _t = 0);

    retf_bool
    oobinline      ();

    retf_void
    set_ooblinline (bool _on);

    retf_int32
    rcvbuf         ();

    retf_void
    set_rcvbuf     (int _s);

    retf_int32
    sndbuf         ();

    retf_void
    set_sndbuf     (int _s);

    retf_int32
    rcvlowat       ();

    retf_void
    set_rcvlowat   (int _s);

    retf_int32
    sndlowat       ();

    retf_void
    set_sndlowat   (int _s);

    retf_void
    rcvtimeo       (struct timeval &_t);

    retf_void
    set_rcvtimeo   (struct timeval _t);

    retf_void
    sndtimeo       (struct timeval &_t);

    retf_void
    set_sndtimeo   (struct timeval _t);

    retf_bool
    reuseaddr      ();

    retf_void
    set_reuseaddr  (bool _on);

    retf_bool
    reuseport      ();

    retf_void
    set_reuseport  (bool _on);

    retf_int32
    type           ();

    retf_int32
    ip_tos         ();

    retf_void
    set_ip_tos     (int _t);

    retf_int32
    ip_ttl         ();

    retf_void
    set_ip_ttl     (int _t);

    retf_bool
    ipv4_disabled  ();

    retf_void
    set_ipv6_only  (bool _on);

    retf_int32
    maxseg         ();

    retf_void
    set_maxseg     (int _s);

    retf_bool
    nodelay        ();

    retf_void
    set_nodelay    (int _s);

protected:
    static bool
    get_flag       (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

    static bool
    get_int        (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

    static bool
    get_linger     (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

    static bool
    get_timeval    (__sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

    static bool
    set_flag       (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

    static bool
    set_int        (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

    static bool
    set_linger     (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

    static bool
    set_timeval    (const __sockopt_val &_val, int _fd, const __sockopts_info &_opt_info);

protected:
    int                          m_fd;

protected:
    static const __sockopts_info opts_entry[SOCKOPTS_TYPE_MAX];
};

__NAMESPACE_END

#endif /* _SOCK_OPTS_H_ */
#ifndef _SOCK_OPTS_H_
#define _SOCK_OPTS_H_

#include <netinet/tcp.h>
#include "define.h"

__NAMESPACE_BEGIN

class sockopts {
protected:
    union __sockopt_val {
        int            i_val;
        long           l_val;
        struct linger  linger_val;
        struct timeval timeval_val;
    } val;

    struct __sockopts_info {
        const char *opt_str;
        int         opt_level;
        int         opt_name;
        bool        (*opt_get)(__sockopt_val &, int, const __sockopts_info &);
        bool        (*opt_set)(__sockopt_val  , int, const __sockopts_info &);
    };

    enum SOCKOPTS_TYPE {
        SOCKOPTS_TYPE_BROADCAST = 0,
        SOCKOPTS_TYPE_DEBUG,
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
        SOCKOPTS_TYPE_SO_REUSEPORT,
        SOCKOPTS_TYPE_SO_TYPE,
        SOCKOPTS_TYPE_IP_TOS,
        SOCKOPTS_TYPE_IP_TTL,
        SOCKOPTS_TYPE_IPV6_UNICAST_HOPS,
        SOCKOPTS_TYPE_IPV6_V6ONLY,
        SOCKOPTS_TYPE_TCP_MAXSEG,
        SOCKOPTS_TYPE_TCP_NODELAY,
        SOCKOPTS_TYPE_MAX
    };

public:
    sockopts       (int _fd);

public:
    bool
    broadcast      ();

    void
    set_broadcast  (bool _on);

    bool
    debug          ();

    void
    set_debug      (bool _on);

    bool
    nonroute       ();

    void
    setnonroute    (bool _on);

    bool
    error          ();

    bool
    keepalive      ();

    void
    setkeepalive   ();

    bool
    linger         ();

    void
    set_linger     (bool _on, int _t = 0);

    void
    oobinline      ();

    void
    set_ooblinline (bool _on);

    int
    rcvbuf         ();

    void
    set_rcvbuf     (int _s);

    int
    sndbuf         ();

    void
    set_sndbuf     (int _s);

    int
    rcvlowat       ();

    void
    set_rcvlowat   (int _s);

    int
    sndlowat       ();

    void
    set_sndlowat   (int _s);

    timestamp
    rcvtimeo       ();

    void
    set_rcvtimeo   (timestamp _t);

    timestamp
    sndtimeo       ();

    void
    set_sndtimeo   (timestamp _t);

    bool
    reuseaddr      ();

    void
    set_reuseaddr  (bool _on);

    bool
    reuseport      ();

    void
    set_reuseport  (bool _on);

    int
    type           ();

    int
    tos            ();

    void
    set_tos        ();

    int
    ip_ttl         ();

    void
    set_ip_ttl     (int _t);


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
    set_flag       (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info);

    static bool
    set_int        (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info);

    static bool
    set_linger     (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info);

    static bool
    set_timeval    (__sockopt_val _val, int _fd, const __sockopts_info &_opt_info);

protected:
    int                    m_fd;

    static __sockopts_info m_opts_entry[SOCKOPTS_TYPE_MAX];
};

__NAMESPACE_END

#endif /* _SOCK_OPTS_H_ */
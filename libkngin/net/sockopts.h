#ifndef _SOCK_OPTS_H_
#define _SOCK_OPTS_H_

#include <netinet/tcp.h>
#include "define.h"
#include "error.h"
#include "socket.h"

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

private:
    sockopts       () = default;

    ~sockopts      () = default;

public:
    static retf_bool
    broadcast      (socket &_s);

    static retf_void
    set_broadcast  (socket &_s, bool _on);

    static retf_bool
    debug          (socket &_s);

    static retf_void
    set_debug      (socket &_s, bool _on);

    static retf_bool
    nonroute       (socket &_s);

    static retf_void
    set_nonroute   (socket &_s, bool _on);

    static retf_int32
    error          (socket &_s);

    static retf_bool
    keepalive      (socket &_s);

    static retf_void
    set_keepalive  (socket &_s, bool _on);

    static retf_void
    linger         (socket &_s, struct linger &_l);

    static retf_void
    set_linger     (socket &_s, bool _on, int _t = 0);

    static retf_bool
    oobinline      (socket &_s);

    static retf_void
    set_ooblinline (socket &_s, bool _on);

    static retf_int32
    rcvbuf         (socket &_s);

    static retf_void
    set_rcvbuf     (socket &_s, int _size);

    static retf_int32
    sndbuf         (socket &_s);

    static retf_void
    set_sndbuf     (socket &_s, int _size);

    static retf_int32
    rcvlowat       (socket &_s);

    static retf_void
    set_rcvlowat   (socket &_s, int _size);

    static retf_int32
    sndlowat       (socket &_s);

    static retf_void
    set_sndlowat   (socket &_s, int _size);

    static retf_void
    rcvtimeo       (socket &_s, struct timeval &_t);

    static retf_void
    set_rcvtimeo   (socket &_s, struct timeval _t);

    static retf_void
    sndtimeo       (socket &_s, struct timeval &_t);

    static retf_void
    set_sndtimeo   (socket &_s, struct timeval _t);

    static retf_bool
    reuseaddr      (socket &_s);

    static retf_void
    set_reuseaddr  (socket &_s, bool _on);

    static retf_bool
    reuseport      (socket &_s);

    static retf_void
    set_reuseport  (socket &_s, bool _on);

    static retf_int32
    type           (socket &_s);

    static retf_int32
    ip_tos         (socket &_s);

    static retf_void
    set_ip_tos     (socket &_s, int _t);

    static retf_int32
    ip_ttl         (socket &_s);

    static retf_void
    set_ip_ttl     (socket &_s, int _t);

    static retf_bool
    ipv4_disabled  (socket &_s);

    static retf_void
    set_ipv6_only  (socket &_s, bool _on);

    static retf_int32
    maxseg         (socket &_s);

    static retf_void
    set_maxseg     (socket &_s, int _size);

    static retf_bool
    nodelay        (socket &_s);

    static retf_void
    set_nodelay    (socket &_s, int _size);

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
    static const __sockopts_info opts_entry[SOCKOPTS_TYPE_MAX];
};

__NAMESPACE_END

#endif /* _SOCK_OPTS_H_ */
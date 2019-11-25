#ifndef _SOCK_OPTS_H_
#define _SOCK_OPTS_H_

#include <netinet/tcp.h>
#include <time.h>
#include "core/define.h"
#include "core/error.h"
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
    broadcast      (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST], _on); }
    static bool
    set_broadcast  (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_BROADCAST]); }
    static bool
    debug          (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG], _on); }
    static bool
    set_debug      (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DEBUG]); }
    static bool
    nonroute       (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE], _on); }
    static bool
    set_nonroute   (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]); }
    static std::error_code
    error          (socket &_s, std::error_code &_ec)
    { /*return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_ERROR], _code);*/ }
    #warning "return std::error_code for socket, out param return std::error_code from system"
    static bool
    keepalive      (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE], _on); }
    static bool
    set_keepalive  (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]); }
    static bool
    linger         (socket &_s, struct linger &_l)
    { bool _ret = sockopts::get_linger(_s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER], _l); return _ret; }
    static bool
    set_linger     (socket &_s, bool _on, int _t /* = 0 */)
    { sockopt_val _val; _val.linger_val = {(int)_on, _t}; return sockopts::set_linger(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_LINGER]); }
    static bool
    oobinline      (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE], _on); }
    static bool
    set_ooblinline (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]); }
    static bool
    rcvbuf         (socket &_s, int &_size)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF], _size); }
    static bool
    set_rcvbuf     (socket &_s, int _size)
    { sockopt_val _val{_size}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVBUF]); }
    static bool
    sndbuf         (socket &_s, int &_size)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF], _size); }
    static bool
    set_sndbuf     (socket &_s, int _size)
    { sockopt_val _val{_size}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDBUF]); }
    static bool
    rcvlowat       (socket &_s, int &_size)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT], _size); }
    static bool
    set_rcvlowat   (socket &_s, int _size)
    { sockopt_val _val{_size}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]); }
    static bool
    sndlowat       (socket &_s, int &_size)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT], _size); }
    static bool
    set_sndlowat   (socket &_s, int _size)
    { sockopt_val _val{_size}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]); }
    static bool
    rcvtimeo       (socket &_s, struct timeval &_t)
    { return sockopts::get_timeval(_s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO], _t); }
    static bool
    set_rcvtimeo   (socket &_s, struct timeval _t)
    { sockopt_val _val; _val.timeval_val = _t; return sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]); }
    static bool
    sndtimeo       (socket &_s, struct timeval &_t)
    { return sockopts::get_timeval(_s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO], _t); }
    static bool
    set_sndtimeo   (socket &_s, struct timeval _t)
    { sockopt_val _val; _val.timeval_val = _t; return sockopts::set_timeval(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]); }
    static bool
    reuseaddr      (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR], _on); }
    static bool
    set_reuseaddr  (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]); }
    static bool
    reuseport      (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT], _on); }
    static bool
    set_reuseport  (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]); }
    static bool
    type           (socket &_s, int &_type)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_TYPE], _type); }
    static bool
    ip_tos         (socket &_s, int &_t)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS], _t); }
    static bool
    set_ip_tos     (socket &_s, int _t)
    { sockopt_val _val{_t}; return sockopts::set_flag(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TOS]); }
    static bool
    ip_ttl         (socket &_s, int &_ttl)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL], _ttl); }
    static bool
    set_ip_ttl     (socket &_s, int _t)
    { sockopt_val _val{_t}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IP_TTL]); }
    static bool
    ipv4_disabled  (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY], _on); }
    static bool
    set_ipv6_only  (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]); }
    static bool
    maxseg         (socket &_s, int &_size)
    { return sockopts::get_int(_s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG], _size); }
    static bool
    set_maxseg     (socket &_s, int _size)
    { sockopt_val _val{_size}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]); }
    static bool
    nodelay        (socket &_s, bool &_on)
    { return sockopts::get_flag(_s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY], _on); }
    static bool
    set_nodelay    (socket &_s, bool _on)
    { sockopt_val _val{_on}; return sockopts::set_int(_val, _s.fd(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]); }

protected:
    static bool
    get_flag       (int _fd, const sockopts_info &_opt_info, bool &_flag);

    static bool
    get_int        (int _fd, const sockopts_info &_opt_info, int &_val);

    static bool
    get_linger     (int _fd, const sockopts_info &_opt_info, struct linger &_linger);

    static bool
    get_timeval    (int _fd, const sockopts_info &_opt_info, struct timeval &_tv);

    static bool
    set_flag       (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

    static bool
    set_int        (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

    static bool
    set_linger     (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

    static bool
    set_timeval    (const sockopt_val &_val, int _fd, const sockopts_info &_opt_info);

protected:
    static const sockopts_info opts_entry[SOCKOPTS_TYPE_MAX];
};

KNGIN_NAMESPACE_K_END

#endif /* _SOCK_OPTS_H_ */
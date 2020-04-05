#ifndef KNGIN_SOCK_OPTS_H
#define KNGIN_SOCK_OPTS_H

#include "kngin/core/define.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/net/socket.h"
#if defined(KNGIN_SYSTEM_WIN32)
#else
#  include <netinet/tcp.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */
#include <ctime>

KNGIN_NAMESPACE_K_BEGIN

namespace sockopts {

union sockopt_val {
  int               i_val;
  long              l_val;
  struct ::linger   linger_val;
  struct ::timeval  timeval_val;
#if defined(KNGIN_SYSTEM__WIN32)
  struct ::tcp_info tcp_info;
#endif /* defined(KNGIN_SYSTEM__WIN32) */
};

struct sockopts_info {
  const char *opt_str;
  int         opt_level;
  int         opt_name;
};

typedef enum {
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
#if !defined(KNGIN_SYSTEM__WIN32)
  SOCKOPTS_TYPE_TCP_INFO,
#endif /* defined(KNGIN_SYSTEM__WIN32) */
  SOCKOPTS_TYPE_MAX
} SOCKOPTS_TYPE;

extern const sockopts_info opts_entry[SOCKOPTS_TYPE_MAX];

inline
bool
get_flag (handle_t h, const sockopts_info &opt_info);

inline
int
get_int (handle_t h, const sockopts_info &opt_info);

struct ::linger
get_linger (handle_t h, const sockopts_info &opt_info);

struct ::timeval
get_timeval (handle_t h, const sockopts_info &opt_info);

#if !defined(KNGIN_SYSTEM__WIN32)
struct ::tcp_info
get_tcp_info (handle_t h, const sockopts_info &opt_info);
#endif /* defined(KNGIN_SYSTEM__WIN32) */

void
set_flag (const sockopt_val &val, handle_t h, const sockopts_info &opt_info);

void
set_int (const sockopt_val &val, handle_t h, const sockopts_info &opt_info);

void
set_linger (const sockopt_val &val, handle_t h, const sockopts_info &opt_info);

void
set_timeval (const sockopt_val &val, handle_t h, const sockopts_info &opt_info);
inline
bool
broadcast (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_BROADCAST]);
}

inline
void
set_broadcast (const socket &s, bool on) {
  sockopt_val val{on};
  set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_BROADCAST]);
}

inline
bool
debug (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_DEBUG]);
}

inline
void
set_debug (const socket &s, bool on) {
  sockopt_val val{on};
  set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_DEBUG]);
}

inline
bool
nonroute (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]);
}

inline
void
set_nonroute (const socket &s, bool on) {
  sockopt_val val{on};
  set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_DONTROUTE]);
}

inline
error_code
error (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_ERROR]);
}

inline
bool
keepalive (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]);
}

inline
void
set_keepalive (const socket &s, bool on) {
  sockopt_val val{on};
  set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_KEEPALIVE]);
}

inline
struct ::linger
linger (const socket &s) {
  return get_linger(s.handle(), opts_entry[SOCKOPTS_TYPE_LINGER]);
}

inline
void
#if defined(KNGIN_SYSTEM__WIN32)
set_linger (const socket &s, bool on, u_short t /* = 0 */) {
#else
set_linger (const socket &s, bool on, int t /* = 0 */) {
#endif /* defined(KNGIN_SYSTEM__WIN32) */
  sockopt_val val;
  val.linger_val = {on, t};
  set_linger(val, s.handle(), opts_entry[SOCKOPTS_TYPE_LINGER]);
}

inline
bool
oob_inline (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]);
}

inline
void
set_oob_inline (const socket &s, bool on) {
  sockopt_val val{on}; set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_OOBINLINE]);
}

inline
int
rcvbuf (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_RCVBUF]);
}

inline
void
set_rcvbuf (const socket &s, int size) {
  sockopt_val val{size};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_RCVBUF]);
}

inline
int
sndbuf (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_SNDBUF]);
}

inline
void
set_sndbuf (const socket &s, int size) {
  sockopt_val val{size};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_SNDBUF]);
}

inline
int
rcvlowat (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]);
}

inline
void
set_rcvlowat (const socket &s, int size) {
  sockopt_val val{size};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_RCVLOWAT]);
}

inline
int
sndlowat (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]);
}

inline
void
set_sndlowat (const socket &s, int size) {
  sockopt_val val{size};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_SNDLOWAT]);
}

inline
struct ::timeval
rcvtimeo (const socket &s) {
  return get_timeval(s.handle(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]);
}

inline
void
set_rcvtimeo (const socket &s, struct ::timeval t) {
  sockopt_val val;
  val.timeval_val = t;
  set_timeval(val, s.handle(), opts_entry[SOCKOPTS_TYPE_RCVTIMEO]);
}

inline
struct ::timeval
sndtimeo (const socket &s) {
  return get_timeval(s.handle(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]);
}

inline
void
set_sndtimeo (const socket &s, struct ::timeval t) {
  sockopt_val val;
  val.timeval_val = t;
  set_timeval(val, s.handle(), opts_entry[SOCKOPTS_TYPE_SNDTIMEO]);
}

inline
bool
reuseaddr (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]);
}

inline
void
set_reuseaddr (const socket &s, bool on) {
  sockopt_val val{on};
  set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_REUSEADDR]);
}

inline
bool
reuseport (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]);
}

inline
void
set_reuseport (const socket &s, bool on) {
  sockopt_val val{on};
  set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_REUSEPORT]);
}

inline
int
type (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_TYPE]);
}

inline
int
ip_tos (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_IP_TOS]);
}

inline
void
set_ip_tos (const socket &s, int t) {
  sockopt_val val{t};
  set_flag(val, s.handle(), opts_entry[SOCKOPTS_TYPE_IP_TOS]);
}

inline
int
ip_ttl (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_IP_TTL]);
}

inline
void
set_ip_ttl (const socket &s, int t) {
  sockopt_val val{t};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_IP_TTL]);
}

inline
bool
ipv4_disabled (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]);
}

inline
void
set_ipv6_only (const socket &s, bool on) {
  sockopt_val val{on};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_IPV6_V6ONLY]);
}

inline
int
maxseg (const socket &s) {
  return get_int(s.handle(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]);
}

inline
void
set_maxseg (const socket &s, int size) {
  sockopt_val val{size};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_TCP_MAXSEG]);
}

inline
bool
nodelay (const socket &s) {
  return get_flag(s.handle(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]);
}

inline
void
set_nodelay (const socket &s, bool on) {
  sockopt_val val{on};
  set_int(val, s.handle(), opts_entry[SOCKOPTS_TYPE_TCP_NODELAY]);
}

#if !defined(KNGIN_SYSTEM__WIN32)
inline
struct ::tcp_info
tcp_info (const socket &s) {
  return get_tcp_info(s.handle(), opts_entry[SOCKOPTS_TYPE_TCP_INFO]);
}
#endif /* defined(KNGIN_SYSTEM__WIN32) */

};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SOCK_OPTS_H */
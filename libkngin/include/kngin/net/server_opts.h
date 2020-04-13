#ifndef KNGIN_SERVER_OPTS_H
#define KNGIN_SERVER_OPTS_H

#include "kngin/core/define.h"
#include <string>

namespace k {

enum socket_type {
  tcp = 0x1,
  udp = 0x2,
};

enum socket_domain {
  ipv4_only = 0x01,
  ipv6_only = 0x02,
  ipv6_ipv4 = 0x03,
};

struct server_opts {
  std::string   name;
  uint16_t      port;
  socket_type   type;
  socket_domain domain;
  int           backlog;
  bool          keep_alive;
};

} /* namespace k */

#endif /* KNGIN_SERVER_OPTS_H */

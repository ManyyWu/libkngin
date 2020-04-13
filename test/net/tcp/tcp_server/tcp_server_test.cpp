#include "kngin/net/tcp/service.h"

#define LISTEN_ADDR "0.0.0.0"
#define SERVER_ADDR "127.0.0.1"
#define LISTEN_PORT 20000

int
main () {
  // demo
  k::event_loop loop;
  k::tcp::service server(loop);
  {
    k::addr(LISTEN_ADDR, LISTEN_PORT);
    std::map<k::handle_t, std::shared_ptr<k::tcp::session>> sessions;

    k::listener listener(server, addr, backlog, [] (k::socket &&s, const k::error_code &ec) {
      if (!ec) {
        auto session = k::tcp::session session(server, socket);
        sessions.insert(std::make_pair(s.handle(), session));
        session.async_read(buf, [] (k::tcp::session &s, in_buffer &buf, const k::error_code &ec) {
          // handle message
        });
      }
    });
  }
  loop.run();

  return 0;
}

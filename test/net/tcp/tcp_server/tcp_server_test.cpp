#include "kngin/core/base/log.h"
#include "kngin/net/service.h"
#include "kngin/net/listener.h"
#include "kngin/net/sockopts.h"

#define LISTEN_ADDR "0.0.0.0"
#define SERVER_ADDR "127.0.0.1"
#define LISTEN_PORT 20000
#define BACKLOG     3

int
main () {
  // create client thread
  k::thread t([] () -> int {
    k::thread::sleep(1000);
    int i = 5;
    while (i-->0) {
      k::socket sock(k::socket::ipv4_tcp);
      try {
        k::address addr(SERVER_ADDR, LISTEN_PORT, false);
        sock.connect(addr);
        log_info("[c]connected to server");

        while (true) {
          char arr[1024];
          size_t len = strlen(fgets(arr, 1024, stdin));
          if (1 == len)
            break;
          k::out_buffer buf(arr, len - 1);
          sock.send(buf, 0);
        }
      } catch (const std::exception &e) {
        log_error("[c]%s", e.what());
      }
      sock.close();
    }
    return 0;
  },"client");

  auto new_conn = [](k::socket &&s, const k::error_code &ec) {
    if (!ec) {
      k::socket sock(std::move(s));
      while (true)
      {
        char arr[1024];
        k::in_buffer buf(arr, 1024);
        k::error_code ec;
        auto ret = sock.read(buf, ec);
        if (0 == ret) {
          log_info("[s]client closed session");
          break;
        }
        else if (ret < 0) {
          log_error("[s]error: %s", ec.message().c_str());
          break;
        }
        buf.write_int8(0);
        log_info("[s]received: %s", buf.begin());
      }
    }
  };

  // demo
  try {
    k::event_loop loop;
    k::service s;
    s.run(1, nullptr);
    {
      k::address addr("0.0.0.0", LISTEN_PORT, false);
      k::socket sock(k::socket::ipv4_tcp);
      k::listener listener(s, sock, addr, BACKLOG, new_conn);
      loop.run();
    }
  } catch (const std::exception &e) {
    log_error("[c]%s", e.what());
  }
  t.join();

  return 0;
}

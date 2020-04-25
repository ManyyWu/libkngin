#include "kngin/core/base/thread.h"
#include "kngin/core/base/log.h"
#include "kngin/net/socket.h"
#include "kngin/net/sockopts.h"
#include <exception>
#include <iostream>
#include <signal.h>

using namespace std;

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 20000

int
main () {
  ::signal(SIGPIPE, SIG_IGN);
  k::thread t([] () -> int {
    k::socket s(k::socket::ipv4_tcp);
    try {
      k::address a("0.0.0.0", SERVER_PORT, false);
      k::sockopts::set_reuseaddr(s, true);
      k::sockopts::set_reuseport(s, true);
      s.bind(a);
      s.listen(3);
      log_info("[s]listening...");
      {
        k::socket cs;
        k::address ca;
        s.accept(ca, cs);
        while (true)
        {
          char arr[1024];
          k::in_buffer buf(arr, 1024);
          k::error_code ec;
          auto ret = cs.read(buf, ec);
          if (0 == ret) {
            log_info("[s]client closed session");
            break;
          }
          else if (ret < 0) {
            log_error("[s]error: %s", ec.message());
            break;
          }
          buf.write_int8(0);
          log_info("[s]received: %s", buf.begin());
        }
      }
    } catch (const exception &e) {
      log_error("[s]%s", e.what());
    }
    s.close();
    return 0;
  }, "server");

  k::thread t1([] () -> int {
    k::thread::sleep(500);
    k::socket s(k::socket::ipv4_tcp);
    try {
      k::address a(SERVER_ADDR, SERVER_PORT, false);
      s.connect(a);
      log_info("[c]connected to server");

      while (true)
      {
        char arr[1024];
        size_t len = strlen(fgets(arr, 1024, stdin));
        if (1 == len)
          break;
        k::out_buffer buf(arr, len - 1);
        s.send(buf, 0);
      }
    } catch (const std::exception &e) {
      log_error("[c]%s", e.what());
    }
    s.close();
    return 0;
  },"client");
  t.join();
  t1.join();

  return 0;
}

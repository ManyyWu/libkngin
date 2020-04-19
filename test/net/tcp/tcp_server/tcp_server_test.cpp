#include "kngin/core/base/log.h"
#include "kngin/net/service.h"
#include "kngin/net/listener.h"
#include "kngin/net/tcp/session.h"
#include "kngin/net/sockopts.h"
#include <string>
#include <map>

#define LISTEN_ADDR "0.0.0.0"
#define SERVER_ADDR "127.0.0.1"
#define LISTEN_PORT 20000
#define BACKLOG     3
#define BUF_SIZE    512

using namespace std;
using namespace std::placeholders;

class tcp_server {
public:
  tcp_server ()
   : sessions_(),
     service_(),
     listen_sock_(k::socket::ipv4_tcp),
     listen_addr_("0.0.0.0", LISTEN_PORT, false),
     listener_(service_, listen_sock_, listen_addr_, BACKLOG,
              std::bind(&tcp_server::on_new_session, this, _1, _2)) {
    service_.run(1, nullptr);
  }

  ~tcp_server () {
    service_.stop();
  }

  void
  on_message (k::tcp::session &session, const k::in_buffer &buf, const k::error_code &ec) {
    auto iter = sessions_.find(&session);
    assert(iter != sessions_.end());
    if (ec) {
      log_error("receive error: %s, name = %s", ec.message().c_str(), iter->second.name.c_str());
      session.close();
      sessions_.erase(iter);
      return;
    }
    if (&buf == &k::tcp::session::eof) {
      log_info("client closed, name = %s", iter->second.name.c_str());
      sessions_.erase(iter);
      return;
    }
    log_info("[s]receive %lld bytes, data= %s, name = %s",
             buf.valid(),
             k::in_buffer(buf.begin(), buf.valid()).dump().c_str(),
             iter->second.name.c_str());

    // post mesage
    const int reply_times = 10;
    session.async_write(k::out_buffer(&reply_times, sizeof(reply_times)), 0);
    for (int i = 0; i < 10; ++i) {
      session.async_write(k::out_buffer(iter->second.arr, buf.valid()), 0);
    }
  }

  void
  on_send (k::tcp::session &session, size_t size, const k::error_code &ec) {
    auto iter = sessions_.find(&session);
    assert(iter != sessions_.end());
    log_info("[s]send: %d bytes, name = %s", size, iter->second.name.c_str());
  }

  void
  on_oob (k::tcp::session &session, uint8_t byte, const k::error_code &ec) {
    auto iter = sessions_.find(&session);
    assert(iter != sessions_.end());
    log_info("[s]receive oob data: %d, name = %s", static_cast<int>(byte), iter->second.name.c_str());
  }

  void
  on_new_session (k::socket &&sock, const k::error_code &ec) {
    if (ec) {
      log_error("listener error: %s", ec.message().c_str());
      return;
    }
    session_data data;
    data.session = std::make_shared<k::tcp::session>(service_,
                                                     std::move(sock),
                                                     std::bind(&tcp_server::on_send, this, _1, _2, _3),
                                                     std::bind(&tcp_server::on_message, this, _1, _2, _3),
                                                     std::bind(&tcp_server::on_oob, this, _1, _2, _3));
    *data.arr = '\0';
    data.name = data.session->name();
    auto iter = sessions_.insert(std::make_pair(data.session.get(), std::move(data)));
    iter.first->second.session->async_read_some(k::in_buffer(iter.first->second.arr, BUF_SIZE));
  }

private:
  struct session_data {
    std::shared_ptr<k::tcp::session> session;
    char arr[BUF_SIZE];
    std::string name;
  };

  std::map<k::tcp::session *, session_data> sessions_;

  k::service service_;

  k::socket listen_sock_;

  k::address listen_addr_;

  k::listener listener_;
};

int
main () {
  try {
    {
      k::event_loop loop;
      tcp_server server;
      loop.run();
    }
  } catch (const std::exception &e) {
    log_error("[s]%s", e.what());
  } catch (...) {
    log_error("[s]unknown exception");
  }

  return 0;
}
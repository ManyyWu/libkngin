#include "kngin/core/base/log.h"
#include "kngin/net/service.h"
#include "kngin/net/listener.h"
#include "kngin/net/tcp/session.h"
#include "kngin/net/sockopts.h"
#include <string>
#include <map>
#include <signal.h>

#define LISTEN_ADDR "0.0.0.0"
#define SERVER_ADDR "127.0.0.1"
#define LISTEN_PORT 20000
#define BACKLOG     3
#define BUF_SIZE    512

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
  on_timer (const k::timer_id &, const k::timestamp &) {
//    log_info("on_timer");
    for (auto &iter : sessions_) {
      if (iter.second.size)
        iter.second.session->async_write(k::out_buffer(iter.second.arr, iter.second.size), 0);
    }
  }

private:
  void
  on_message (k::tcp::session &session, const k::in_buffer &buf, const k::error_code &ec) {
    if (ec) {
      on_error(session, ec);
      return;
    }

    auto iter = sessions_.find(&session);
    assert(iter != sessions_.end());
    log_info("[s]receive %lld bytes, data= %s, name = %s",
             buf.valid(),
             k::in_buffer(buf.begin(), buf.valid()).dump().c_str(),
             iter->second.name.c_str());

    const int reply_times = BUF_SIZE / buf.valid();
    iter->second.size = buf.valid() * reply_times;
    int arr = BUF_SIZE / buf.valid() * 10;
    session.async_write(k::out_buffer(&arr, sizeof(arr)), 0);
    k::in_buffer temp_buf(iter->second.arr, iter->second.size);
    while (temp_buf.writeable())
      temp_buf.write(buf.begin(), buf.valid());
  }

  void
  on_send (k::tcp::session &session, size_t size, const k::error_code &ec) {
    if (ec) {
      on_error(session, ec);
      return;
    }

    auto iter = sessions_.find(&session);
    assert(iter != sessions_.end());
    log_info("[s]send: %d bytes, name = %s", size, iter->second.name.c_str());
  }

  void
  on_oob (k::tcp::session &session, uint8_t byte, const k::error_code &ec) {
    if (ec) {
      on_error(session, ec);
      return;
    }

    auto iter = sessions_.find(&session);
    assert(iter != sessions_.end());
    log_info("[s]receive oob data: %d, name = %s", static_cast<int>(byte), iter->second.name.c_str());
  }

  void
  on_new_session (k::socket &&sock, const k::error_code &ec) {
    if (ec) {
      log_error("listener error: %s", ec.message());
      return;
    }
    session_data data;
    data.session = std::make_shared<k::tcp::session>(service_,
                                                     std::move(sock),
                                                     std::bind(&tcp_server::on_send, this, _1, _2, _3),
                                                     std::bind(&tcp_server::on_message, this, _1, _2, _3),
                                                     std::bind(&tcp_server::on_oob, this, _1, _2, _3));
    *data.arr = '\0';
    data.name = data.session->ip_address() + ":" + std::to_string(data.session->port());
    data.size = 0;
    auto iter = sessions_.insert(std::make_pair(data.session.get(), std::move(data)));
    log_info("accept new session: %s, online sessions:%d", iter.first->second.name.c_str(), sessions_.size());
    iter.first->second.session->async_read_some(k::in_buffer(iter.first->second.arr, BUF_SIZE));
  }

  void
  on_error (k::tcp::session &session, const k::error_code &ec) {
    auto iter = sessions_.find(&session);
    assert(iter != sessions_.end());
    if (ec != k::KNGIN_EOF)
      log_error("%s, name = %s", ec.message(), iter->second.name.c_str());
    else
      log_info("client closed, name = %s", iter->second.name.c_str());
    session.close([&] (k::tcp::session &session) {
      auto iter = sessions_.find(&session);
      assert(iter != sessions_.end());
      sessions_.erase(iter);
      log_info("online sessions: %d", sessions_.size());
    });
  }

private:
  struct session_data {
    std::shared_ptr<k::tcp::session> session;
    char arr[BUF_SIZE];
    size_t size;
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
  sigset_t signal_mask;
  sigemptyset(&signal_mask);
  sigaddset(&signal_mask, SIGPIPE);
  if(pthread_sigmask(SIG_BLOCK, &signal_mask, NULL) == -1)
    perror("SIGPIPE");

  try {
    {
      k::event_loop loop;
      tcp_server server;
      loop.run_every(1000, std::bind(&tcp_server::on_timer, &server, _1, _2));
      loop.run();
    }
  } catch (const std::exception &e) {
    log_error("[s]%s", e.what());
  } catch (...) {
    log_error("[s]unknown exception");
  }

  return 0;
}

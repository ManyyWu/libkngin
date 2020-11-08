#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_CONNECTOR)

#include "kngin/core/base/system_error.h"
#include "kngin/core/base/exception.h"
#include "detail/net/impl/posix_connector.h"
#include "kngin/net/socket.h"
#include "kngin/net/sockopts.h"

namespace k::detail::impl {

posix_connector::posix_connector (service &s, inet_protocol proto, const address &addr)
 : loop_(s.next_loop()),
   socket_(INVALID_HANDLE),
   addr_(addr),
   handler_(),
   ev_(/*sock.handle(), [this] (event_loop &loop, int events) { on_events(loop, events); }*/),
   state_(0) {
}

posix_connector::~posix_connector () {
}

void
posix_connector::async_connect (connect_handler &&handler) {
  if (state_ & (state_connected | state_connecting))
    return;

  ev_.enable_write();
  loop_.register_event(ev_);
}

void
posix_connector::on_events (event_loop &loop, int events) {
  assert(&loop_ == &loop);
  TRY()
    if (events & (epoll_event::event_type_error | epoll_event::event_type_write)) {
      on_write();
    } else {
      assert(0);
    }
  CATCH_ERROR("posix_connector::on_events")
}

void
posix_connector::on_write () {

}

/*
void
posix_connector::connect (error_code &ec) noexcept {
  int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
  int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
  int savedErrno = (ret == 0) ? 0 : errno;
  switch (savedErrno)
  {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
      connecting(sockfd);
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
      retry(sockfd);
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
      LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);
      break;

    default:
      LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);
      // connectErrorCallback_();
      break;
  }
}
 */

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_CONNECTOR) */

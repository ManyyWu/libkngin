#ifndef KNGIN_POSIX_TCP_OSTREAM_H
#define KNGIN_POSIX_TCP_OSTREAM_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_TCP_SESSION)

#include "kngin/core/base/buffer.h"
#include "kngin/core/base/mutex.h"
#include "kngin/net/socket.h"
#include <deque>
#include <vector>
#include <atomic>

namespace k::detail::impl {

namespace tcp {

class posix_session;
class posix_session::ostream : noncopyable {
  friend class posix_session;

public:
  typedef k::tcp::session::write_handler write_handler;

  ostream (posix_session &session, socket &sock, write_handler &&w_cb);

  ~ostream () noexcept;

  void
  async_write (const out_buffer &buf, int flags);

  void
  clear ();

  size_t
  remaining () const noexcept {
    return windex_ - rindex_;
  }

private:
  void
  on_write ();

  void
  write_oob ();

  void
  on_complete (size_t size, const error_code &ec);

  void
  append_buffer (const out_buffer &buf);

private:
  posix_session &session_;

  write_handler write_handler_;

  std::vector<uint8_t> oobq_;

  std::vector<uint8_t *> buffers_;

  std::atomic_size_t rindex_;

  std::atomic_size_t windex_;

  mutex buffers_mutex_;

  std::atomic<uint8_t *> next_buffer_;

  std::atomic_bool complete_;
};

} /* namespace tcp */

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_TCP_SESSION) */

#endif /* KNGIN_POSIX_TCP_OSTREAM_H */

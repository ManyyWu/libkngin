#ifndef KNGIN_STREAM_H
#define KNGIN_STREAM_H

#include "kngin/core/define.h"
#include "kngin/core/net/buffer.h"
#include <functional>
#include <deque>

namespace k::detail {

class event_loop;
class stream : noncopyable {
public:
  stream () = delete;

  stream (event_loop &loop);

  ~stream () noexcept;

  void
  send (out_buffer buf);

  void
  recv ();

private:
  void
  on_event (event_loop &loop, int events);

  void
  on_read ();

  void
  on_write ();

  void
  on_error ();

  void
  on_close ();

  void
  on_oob ();

private:
  class in_context;
  class out_context;
  typedef std::deque<in_context *> in_context_q;
  typedef std::deque<out_context *> out_context_q;

  handle handle_;

  in_context_q in_ctxq;

  out_context_q out_ctxq;
};

} /* namespace k::detail */

#endif /* KNGIN_STREAM_H */

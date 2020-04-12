#ifndef KNGIN_LOGGER_H
#define KNGIN_LOGGER_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/logfile.h"
#include <atomic>
#include <deque>

// "YYYY/MM/DD hh:mm:ss.ms | type | func[file:line] | fmt \n"
#define KNGIN_LOG_FORMAT(type, fmt) " | " type " | %s[%s:%d] | " fmt "\n"

// "YYYY/MM/DD hh:mm:ss.ms | type | fmt \n"
#define KNGIN_LOG_FORMAT_NOLINE(type, fmt) " | " type " | " fmt "\n"

// "****** func[file:line] ******\n"
#define KNGIN_LOG_ASSERT_FORMAT " | ASSERT  | %s[%s:%d] | ****** %s *******\n"

namespace k {

class mutex;
class cond;
class thread;
class logger : public noncopyable {
  friend class logfile;
  friend logger &query_logger ();

private:
  logger ();

  void
  init ();

  void
  deinit () noexcept;

  void
  post_log (log_level level, logfile &file,
            std::string &&data, size_t size);

#if defined(KNGIN_USE_ASYNC_LOGGER)
  int
  log_thread () noexcept;
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */

  static
  const char *
  get_datetime (char datetime[], size_t size) noexcept;

  static
  void
  format_log (log_level level, std::string &result,
              const char *fmt, va_list vl);

  static
  void
  write_log (log_level level, logfile &file,
             std::string &data, size_t size);

  static
  void
  write_logfile (const char *file, log_level level,
                 const char *data, size_t len) noexcept;

  static
  void
  write_stderr (log_level level, const char *data, size_t len) noexcept;

  static
  void
  write_stderr2 (log_level level, const char *data, ...) noexcept;

public:
  ~logger () noexcept;

  static
  logfile &
  add_logfile (const char *file, int mode, log_callback &&cb);

private:
#if defined(KNGIN_USE_ASYNC_LOGGER)
  struct async_log_data {
    log_level level;
    logfile &file;
    std::string data;
    size_t size;
  };

  typedef std::deque<async_log_data> async_log_dataq;

  thread *write_thr_;

  mutex *mutex_;

  cond *cond_;

  std::atomic_bool stop_;

  async_log_dataq log_dataq_;
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
  typedef std::vector<logfile *> logfile_vec;

  logfile_vec files_;
};

extern const size_t g_path_prefix_size;
extern logger &query_logger ();

} /* namespace k */

#endif /* KNGIN_LOGGER_H */
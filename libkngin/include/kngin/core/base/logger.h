#ifndef KNGIN_LOGGER_H
#define KNGIN_LOGGER_H

#include "kngin/core/define.h"
#include "kngin/core/base/logfile.h"
#include "kngin/core/base/noncopyable.h"
#include <atomic>
#include <deque>

// "YYYY/MM/DD hh:mm:ss.ms | type | func[file:line] | fmt \n"
#define KNGIN_LOG_FORMAT(type, fmt) " | " type " | %s[%s:%d] | " fmt "\n"

// "YYYY/MM/DD hh:mm:ss.ms | type | fmt \n"
#define KNGIN_LOG_FORMAT_NOLINE(type, fmt) " | " type " | " fmt "\n"


// "****** func[file:line] ******\n"
#define KNGIN_LOG_ASSERT_FORMAT " | ASSERT  | %s[%s:%d] | ****** %s *******\n"

KNGIN_NAMESPACE_K_BEGIN

class mutex;
class cond;
class thread;
class logger : public noncopyable {
  friend logger& get_logger ();
private:
  typedef size_t size_type;

  logger ();

  ~logger () noexcept;

private:
  void
  init ();

  static
  const char *
  get_datetime (char datetime[], size_type size) noexcept;

  static
  void
  write_logfile (const char *file, KNGIN_LOG_LEVEL level,
                 const char *data, size_type len) noexcept;

  static
  void
  write_stderr (KNGIN_LOG_LEVEL level, const char *data, size_type len) noexcept;

  static
  void
  write_stderr2 (KNGIN_LOG_LEVEL level, const char *data, ...) noexcept;

public:
  static
  void
  format_log (KNGIN_LOG_LEVEL level, std::string &result,
              const char *fmt, va_list vl);

  void
  write_log (KNGIN_LOG_LEVEL level, log_file &file,
             std::string &&data, size_type size);

private:
#if defined(KNGIN_USE_ASYNC_LOGGER)
  struct async_log_data {
    log_file &file;
    std::string data;
    size_type size;
  };

  typedef std::deque<async_log_data> async_log_dataq;

  static thread *write_thr_;

  static mutex *mutex_;

  static cond *cond_;

  static std::atomic_bool stop_;

  static async_log_dataq log_dataq_;
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
};

extern const size_t g_path_prefix_size;
extern logger &log;

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGGER_H */
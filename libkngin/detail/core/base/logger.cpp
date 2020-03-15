#include "kngin/core/base/logger.h"
#include "kngin/core/base/string.h"
#include "kngin/core/base/thread.h"
#include "kngin/core/base/cond.h"
#include "kngin/core/base/mutex.h"
#include "kngin/core/base/memory.h"
#include <cstring>
#include <cstdarg>
#include <memory>
#include <iostream>

#define KNGIN_LOG_BUF_SIZE       4096
#define KNGIN_LOG_FILE_MAX_SIZE  20 * 1024 * 1024 // 20M

// "YYYY/MM/DD hh:mm:ss"
#define KNGIN_LOG_DATETIME_FORMT "%04d/%02d/%02d %02d:%02d:%02d"
#define KNGIN_LOG_DATETIME_LEN   20

// "name_YYYY-MM-dd.log"
#define KNGIN_LOG_FILENAME_FORMT "%s_%04d-%02d-%02d.log"

// color
#if !defined(KNGIN_SYSTEM_WIN32)
#define KNGIN_LOG_COLOR_NONE    "\033[0m"
#define KNGIN_LOG_COLOR_FATAL   "\033[01;32;41m"
#define KNGIN_LOG_COLOR_ERROR   "\033[01;37;43m"
#define KNGIN_LOG_COLOR_WARNING "\033[01;31;49m"
#define KNGIN_LOG_COLOR_INFO    "\033[01;32;49m"
#define KNGIN_LOG_COLOR_DEBUG   ""
#endif /* !defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

// length of project path prefix
const size_t g_path_prefix_size = (::strlen(__FILE__) -
    ::strlen("libkngin/detail/core/base/logger.cpp"));

// global logger
std::unique_ptr<logger> g_logger_ptr = nullptr;
logger &
query_logger () {
  if (!g_logger_ptr)
     g_logger_ptr = std::unique_ptr<logger>(new logger());
  return *g_logger_ptr;
}
logger &g_logger = query_logger();

// color prefix and suffix for logger
#if defined(KNGIN_SYSTEM_WIN32)
# define color_prefix_str(level) ""
# define color_suffix_str()      ""
#else
static const char * const log_color_prefix_entry[
    static_cast<int>(log_level::log_level_max)] = {
  KNGIN_LOG_COLOR_FATAL,
  KNGIN_LOG_COLOR_ERROR,
  KNGIN_LOG_COLOR_WARNING,
  KNGIN_LOG_COLOR_INFO,
  KNGIN_LOG_COLOR_DEBUG,
};
# define color_prefix_str(level) log_color_prefix_entry[static_cast<int>(level)]
# define color_suffix_str()      KNGIN_LOG_COLOR_NONE
#endif /* defined(KNGIN_SYSTEM_WIN32) */

logger::logger ()
 :
#if defined(KNGIN_USE_ASYNC_LOGGER)
   write_thr_(nullptr),
   mutex_(nullptr),
   cond_(nullptr),
   stop_(false),
   log_dataq_(),
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
   files_(){
  init();
}

logger::~logger () noexcept {
  TRY()
    deinit();
  IGNORE()
}

void
logger::init () {
#if defined(KNGIN_USE_ASYNC_LOGGER)
  try {
    mutex_ = new mutex();
    cond_ = new cond(*mutex_);
    write_thr_ = new thread(std::bind(&logger::log_thread, this),
                            "log_thread");
  } catch (...) {
    safe_release(cond_);
    safe_release(mutex_);
    safe_release(write_thr_);
    throw;
  }
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
}

void
logger::deinit () noexcept {
#if defined(KNGIN_USE_ASYNC_LOGGER)
  {
    stop_ = true;
    mutex::scoped_lock lock(*mutex_);
  }
  cond_->signal();
  if (write_thr_->joinable())
    write_thr_->join();

  safe_release(cond_);
  safe_release(mutex_);
  safe_release(write_thr_);
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
  for (auto &iter : g_logger.files_)
    safe_release(iter);
}

void
logger::post_log (log_level level, logfile &file,
                  std::string &&data, size_type size) {
#if defined(KNGIN_USE_ASYNC_LOGGER)
  {
    mutex::scoped_lock lock(*mutex_);
    log_dataq_.push_front({level, file, std::move(data), size});
  }
  cond_->signal();
#else
  logger::write_log(level, file, data, size);
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */
}

#if defined(KNGIN_USE_ASYNC_LOGGER)
int
logger::log_thread () noexcept {
  try {
    async_log_data *data = nullptr;
    while (true) {
      {
        // wait for data
        mutex::scoped_lock lock(*mutex_);
        if (data) {
          log_dataq_.pop_back();
          data = nullptr;
        }
        while (!stop_ and log_dataq_.empty())
          cond_->timed_wait(KNGIN_ASYNC_LOGGER_TIMEOUT);
        if (stop_ and log_dataq_.empty())
          break;

        // get next
        data = &log_dataq_.back();
        assert(data);
      }
      // write log
      logger::write_log(data->level, data->file, data->data, data->size);
    }
  } catch (const std::exception &e) {
    std::cerr << "catch an exception in log_thread: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "catch an unknown exception in log_thread" << std::endl;
  }

  return 0;
}
#endif /* defined(KNGIN_USE_ASYNC_LOGGER) */

const char *
logger::get_datetime (char datetime[], size_type size) noexcept {
  assert(size >= KNGIN_LOG_DATETIME_LEN);
  auto t = ::time(nullptr);
  struct ::tm tm;
  get_localtime(&tm, &t);
  ::snprintf(datetime, KNGIN_LOG_DATETIME_LEN,
             KNGIN_LOG_DATETIME_FORMT,
             tm.tm_year + 1900, tm.tm_mon, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
  return datetime;
}

void
logger::format_log (log_level level, std::string &result,
                    const char *fmt, va_list vl) {
  format_string(result, fmt, vl, KNGIN_LOG_DATETIME_LEN - 1, 0);
  char datetime[KNGIN_LOG_DATETIME_LEN];
  logger::get_datetime(datetime, KNGIN_LOG_DATETIME_LEN);
  ::memcpy(result.data(), datetime, KNGIN_LOG_DATETIME_LEN - 1);
}

void
logger::write_log (log_level level, logfile &file,
                   std::string &data, size_type size) {
  if (file.mode_ & KNGIN_LOG_MODE_FILE)
    logger::write_logfile(file.file_.c_str(), level, data.c_str(), size);
  if (file.mode_ & KNGIN_LOG_MODE_STDERR)
    logger::write_stderr(level, data.c_str(), size);
  TRY()
    if (file.mode_ & KNGIN_LOG_MODE_CALLBACK and file.cb_)
      file.cb_(file.file_.c_str(), level, data.c_str(), size);
  CATCH_FATAL("logger::write_log()")
}

void
logger::write_logfile (const char *file, log_level level,
                       const char *data, size_type len) noexcept {
  assert(file);
  assert(data);
  assert(len);

  static char  filename[FILENAME_MAX];
  size_type    ret = 0;
  tm           tm;
  time_t       t = ::time(nullptr);
  FILE *       fplog = nullptr;

  get_localtime(&tm, &t);
  ::snprintf(filename, FILENAME_MAX, KNGIN_LOG_FILENAME_FORMT, file,
             tm.tm_year + 1900, tm.tm_mon, tm.tm_mday);
  fplog = ::fopen(filename, "a");
  if (!fplog) {
    logger::write_stderr2(log_level::log_level_fatal,
                          "failed to open \"%s\", %s[%#x]",
                          filename, ::strerror(errno), errno);
    return;
  }

  ret = ::fwrite(data, 1, len, fplog);
  if (ret < 0) {
    logger::write_stderr2(log_level::log_level_fatal,
                          "failed to write log to \"%s\", %s[%#x]",
                          filename, ::strerror(errno), errno);
    goto fail;
  }
fail:
  ::fclose(fplog);
}

void
logger::write_stderr (log_level level, const char *data, size_type len) noexcept {
  assert(data);
  assert(len);

#if defined(KNGIN_SYSTEM_WIN32)
  ::fwrite(data, 1, len, stderr);
#else
  ::fputs(color_prefix_str(level), stderr);
  ::fwrite(data, 1, len, stderr);
  ::fputs(color_suffix_str(), stderr);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

void
logger::write_stderr2 (log_level level, const char *fmt, ...) noexcept {
  assert(fmt);
  va_list vl;
  va_start(vl, fmt);

#if defined(KNGIN_SYSTEM_WIN32)
  ::vfprintf(stderr, KNGIN_LOG_BUF_SIZE, fmt, vl);
#else
  ::fputs(color_prefix_str(level), stderr);
  ::vfprintf(stderr, fmt, vl);
  ::fputs(color_suffix_str(), stderr);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
  va_end(vl);
}

logfile &
logger::add_logfile (const char *file, int mode, log_callback &&cb) {
  logfile *ptr = nullptr;
  ptr = new logfile(file, mode, std::move(cb));
  try {
    g_logger.files_.push_back(ptr);
  } catch (...) {
    safe_release(ptr);
  }
  return *g_logger.files_.back();
}

KNGIN_NAMESPACE_K_END


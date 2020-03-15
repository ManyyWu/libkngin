#ifndef KNGIN_CALLBACK_H
#define KNGIN_CALLBACK_H

#include "kngin/core/define.h"
#include <functional>

KNGIN_NAMESPACE_K_BEGIN

enum class log_level;
class timer_id;
class timestamp;

typedef void (log_callback_pfn) (const char *, log_level, const char *, size_t size);
typedef void (void_void_pfn) (void);
typedef void (timeout_pfn) (const timer_id &, const timestamp &);

#if defined(ENABLE_USE_STD_FUNCTION_LOG_CALLBACK)
  typedef std::function<log_callback_pfn> log_callback;
#else
  typedef log_callback_pfn * log_callback;
#endif /* defined(ENABLE_USE_STD_FUNCTION_LOG_CALLBACK) */

#if defined(ENABLE_USE_STD_FUNCTION_EVENT_LOOP_TASK)
  typedef std::function<void_void_pfn> task;
#else
  typedef void_void_pfn * task;
#endif /* defined(ENABLE_USE_STD_FUNCTION_EVENT_LOOP_TASK) */

#if defined(KNGIN_USE_STD_FUNCTION_TIMER_HANDLER)
  typedef std::function<timeout_pfn> timeout_handler;
#else
  typedef timeout_pfn * timeout_handler;
#endif /* defined(KNGIN_USE_STD_FUNCTION_TIMER_HANDLER) */

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_CALLBACK_H */
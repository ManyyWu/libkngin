#ifndef KNGIN_CALLBACK_H
#define KNGIN_CALLBACK_H

#include "kngin/core/define.h"
#include <functional>
#include <string>

KNGIN_NAMESPACE_K_BEGIN

enum class KNGIN_LOG_LEVEL;

typedef int (thread_proc_pfn) (void *);
typedef void (log_callback_pfn) (const char *, KNGIN_LOG_LEVEL, const char *, size_t size);
typedef void (void_void_pfn) (void);

#if defined(KNGIN_USE_STD_FUNCTION_THREAD_PROC)
  typedef std::function<thread_proc_pfn> thread_proc;
#else
  typedef thread_proc_pfn * thread_proc;
#endif /* defined(KNGIN_USE_STD_FUNCTION_THREAD_PROC) */

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

#if defined(KNGIN_USE_STD_FUNCTION_EVENT_LOOP_HANDLER)
  typedef std::function<void_void_pfn> event_loop_handler;
#else
  typedef void_void_pfn * event_loop_handler;
#endif /* defined(KNGIN_USE_STD_FUNCTION_EVENT_LOOP_HANDLER) */


KNGIN_NAMESPACE_K_END

#endif /* KNGIN_CALLBACK_H */
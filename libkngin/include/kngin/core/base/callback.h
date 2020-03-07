#ifndef KNGIN_CALLBACK_H
#define KNGIN_CALLBACK_H

#include <functional>
#include <string>

KNGIN_NAMESPACE_K_BEGIN

enum class KNGIN_LOG_LEVEL;

typedef int (int_voidptr_pfn) (void *);
typedef void (void_string_ref_pfn) (std::string &);
typedef void (void_ccp_KNGIN_LOG_LEVEL_ccp_size) (const char *, KNGIN_LOG_LEVEL, const char *, size_t size);

#if defined(KNGIN_THREAD_USE_STD_FUNCTION)
  typedef std::function<int_voidptr_pfn> thread_proc;
#else
  typedef int_void_pfn * thread_proc;
#endif /* defined(KNGIN_THREAD_USE_STD_FUNCTION) */

#if defined(KNGIN_LOG_CALLBACK_USE_STD_FUNCTION)
  typedef std::function<void_KNGIN_LOG_LEVEL_ccp_size> log_callback;
#else
  typedef void_ccp_KNGIN_LOG_LEVEL_ccp_size *log_callback;
#endif /* defined(KNGIN_LOG_CALLBACK_USE_STD_FUNCTION) */

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_CALLBACK_H */
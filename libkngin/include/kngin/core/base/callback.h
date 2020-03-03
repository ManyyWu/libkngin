#ifndef KNGIN_CALLBACK_H
#define KNGIN_CALLBACK_H

#include <functional>
#include <string>

KNGIN_NAMESPACE_K_BEGIN

typedef int (int_void_pfn)(void);
typedef void (void_std_string_ref_pfn)(std::string &);

#if defined(KNGIN_THREAD_USE_STD_FUNCTION)
  typedef std::function<int_void_pfn> thread_proc;
#else
  typedef int_void_pfn * thread_proc;
#endif /* defined(KNGIN_THREAD_USE_STD_FUNCTION) */

KNGIN_NAMESPACE_K_END


#endif /* KNGIN_CALLBACK_H */
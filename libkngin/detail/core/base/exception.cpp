#include "kngin/core/base/exception.h"
#if defined(KNGIN_SYSTEM_WIN32)
#else
#include <execinfo.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

void
exception::dump_stack () {
#if defined(KNGIN_SYSTEM_WIN32)
#else
  void * array[100];
  int    size = ::backtrace(array, 100);
  char **statcks = ::backtrace_symbols(array, size);

  dump_str_ += "invocation stack: ";
  if (size and statcks)
    for (int i = 0; i < size; ++i)
      if (statcks[i])
        dump_str_ += std::string("\n") + statcks[i];
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

KNGIN_NAMESPACE_K_END

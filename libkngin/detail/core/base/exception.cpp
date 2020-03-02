#include "kngin/core/base/exception.h"
#if defined(KNGIN_SYSTEM_WIN32)
#else
#include <execinfo.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

#define KNGIN_DUMP_LINE_SIZE 512
#define KNGIN_DUMP_LINE 128

KNGIN_NAMESPACE_K_BEGIN

void
exception::dump_stack () {
#if defined(KNGIN_SYSTEM_WIN32)
#else
  void * array[KNGIN_DUMP_LINE];
  int    size = ::backtrace(array, sizeof(array));
  char **statcks = ::backtrace_symbols(array, size);
  if (!size or !statcks)
    return;

  dump_str_.reserve(size * KNGIN_DUMP_LINE_SIZE);
  dump_str_ += "invocation stack: ";
  if (size and statcks) {
    for (int i = 0; i < size; ++i) {
      if (statcks[i]) {
        dump_str_ += std::string("\n");
        dump_str_ += + statcks[i];
      }
    }
  }
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

KNGIN_NAMESPACE_K_END

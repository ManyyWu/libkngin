#include "kngin/core/base/exception.h"
#if defined(KNGIN_SYSTEM_WIN32)
#else
#include <execinfo.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

#define KNGIN_DUMP_LINE_SIZE 512
#define KNGIN_DUMP_LINE 128

namespace k {

void
exception::dump_stack (std::string &str) {
  str.clear();
#if defined(KNGIN_SYSTEM_WIN32)
#else
  void * array[KNGIN_DUMP_LINE];
  int    size = ::backtrace(array, sizeof(array));
  char **statcks = ::backtrace_symbols(array, size);
  if (!size or !statcks)
    return;

  str.reserve(size * KNGIN_DUMP_LINE_SIZE);
  str += "invocation stack: ";
  if (size and statcks) {
    for (int i = 0; i < size; ++i) {
      if (statcks[i]) {
        str += std::string("\n");
        str += + statcks[i];
      }
    }
  }
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

} /* namespace k */

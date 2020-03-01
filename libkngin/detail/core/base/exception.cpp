#include "kngin/core/base/exception.h"
#if defined(KNGIN_SYSTEM_WIN32)
#else
#include <execinfo.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

void
exception::dump_stack ()
{
#if defined(KNGIN_SYSTEM_WIN32)
#else
  void * _array[100];
  int    _size = ::backtrace(_array, 100);
  char **_stacks = ::backtrace_symbols(_array, _size);

  dump_str_ += "invocation stack: ";
  if (_size and _stacks)
    for (int _i = 0; _i < _size; ++_i)
      if (_stacks[_i])
        dump_str_ += std::string("\n") + _stacks[_i];
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

KNGIN_NAMESPACE_K_END

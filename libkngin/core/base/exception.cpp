#ifdef _WIN32
#else
#include <execinfo.h>
#endif
#include <string>
#include "core/base/exception.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/base/exception.cpp"

KNGIN_NAMESPACE_K_BEGIN

void
exception::dump_stack ()
{
    void * _array[100];
    int    _size = ::backtrace(_array, 100);
    char **_stacks = ::backtrace_symbols(_array, _size);

    m_dump_str += "invocation stack: ";
    if (_size and _stacks)
        for (int i = 0; i < _size; i++)
            if (_stacks[i])
                m_dump_str += std::string("\n") + _stacks[i];
}

KNGIN_NAMESPACE_K_END

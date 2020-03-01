#ifdef _WIN32
#else
#include <execinfo.h>
#endif
#include <string>
#include "core/base/exception.h"

#define KNGIN_FILENAME "libkngin/core/base/exception.cpp"

KNGIN_NAMESPACE_K_BEGIN

void
exception::dump_stack ()
{
#ifdef _WIN32
#else
    void * array[100];
    int    size = ::backtrace(array, 100);
    char **stacks = ::backtrace_symbols(array, size);

    m_dump_str += "invocation stack: ";
    if (size and stacks)
        for (int i = 0; i < size; ++i)
            if (stacks[i])
                m_dump_str += std::string("\n") + stacks[i];
#endif
}

KNGIN_NAMESPACE_K_END

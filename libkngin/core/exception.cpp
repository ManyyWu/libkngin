#ifdef _WIN32
#else
#include <execinfo.h>
#endif
#include <string>
#include "core/exception.h"
#include "core/common.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/exception.cpp"

KNGIN_NAMESPACE_K_BEGIN

exception::exception (const char *_what)
    : m_what(_what),
      m_dump_str()
{
    dump_stack();
}

void
exception::dump_stack ()
{
    void * _array[100];
    int    _size = ::backtrace(_array, 100);
    char **_stacks = ::backtrace_symbols(_array, _size);

    m_dump_str += "dump: ";
    if (_size && _stacks)
        for (int i = 0; i < _size; i++)
            if (_stacks[i])
                m_dump_str += std::string("\n") + _stacks[i];
}

KNGIN_NAMESPACE_K_END

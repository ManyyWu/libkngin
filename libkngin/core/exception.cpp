#ifdef _WIN32
#else
#include <execinfo.h>
#endif
#include <exception>
#include <stdexcept>
#include <string>
#include "exception.h"
#include "common.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/exception.cpp"

__NAMESPACE_BEGIN

exception::exception (const char *_what)
    : m_str(_what)
{
//#ifndef NDEBUG
//    check(!"exception");
//#endif
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

__NAMESPACE_END

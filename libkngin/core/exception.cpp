#include <exception>
#include <stdexcept>
#include <string>
#include "define.h"
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
#ifndef NDEBUG
    check(!"exception");
#endif
    this->dump_stack();
}

void
exception::dump_stack ()
{
    m_dump_str += "dump: ";
}

__NAMESPACE_END

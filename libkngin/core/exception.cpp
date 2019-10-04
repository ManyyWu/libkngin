#include <exception>
#include <stdexcept>
#include <string>
#include "define.h"
#include "exception.h"

__NAMESPACE_BEGIN

exception::exception (const char *_what)
    : m_str(_what)
{
    // dump
    this->dump_stack();
}

exception::~exception ()
{
}

const std::string &
exception::what () const
{
    return m_str;
}

const std::string &
exception::dump () const
{
    return m_dump_str;
}

void
exception::dump_stack ()
{
    m_dump_str += "dump: ";
}

__NAMESPACE_END

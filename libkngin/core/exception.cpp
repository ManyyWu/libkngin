#include <exception>
#include <stdexcept>
#include <string>
#include "define.h"
#include "exception.h"

__NAMESPACE_BEGIN

logic_exception::logic_exception (std::string _what, const char *_file, int _line) __EXP
    : std::logic_error(std::string("[") +  _file + ":" + std::to_string(_line) + "] " + _what)
{
    // dump
    this->dump_stack();
}

logic_exception::~logic_exception () __NOEXP
{
}

const char*
logic_exception::what () const __NOEXP
{
    return logic_error::what();
}

const char *
logic_exception::dump () const __NOEXP
{
    return m_dump_str.c_str();
}

void
logic_exception::dump_stack () __EXP
{
}

__NAMESPACE_END
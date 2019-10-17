#include <exception>
#include <stdexcept>
#include <string>
#include "define.h"
#include "exception.h"

__NAMESPACE_BEGIN

void
exception::dump_stack ()
{
    m_dump_str += "dump: ";
}

__NAMESPACE_END

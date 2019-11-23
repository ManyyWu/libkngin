#include <cassert>
#include "exception.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/error.cpp"

KNGIN_NAMESPACE_K_BEGIN
/*
const __sys_err::__err_info
__sys_err::__err_info_entry[__sys_err::__sys_err_num] = {
//    {E_SUCCESS,       "Success"                     },
//    {E_MAX,           "Max error code"              }
};

const char *
__sys_err::what (uint32_t _code)
{
    assert(_code == __sys_err::__err_info_entry[_code]._code);
    if (_code >= E_MAX)
        throw k::exception("out of range");
    return __sys_err::__err_info_entry[_code]._str;
}
*/
KNGIN_NAMESPACE_K_END

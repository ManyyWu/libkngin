#include <cassert>
#include "define.h"
#include "error.h"
#include "exception.h"

__NAMESPACE_BEGIN

const __sys_err::__err_info
__sys_err::__err_info_entry[__sys_err::__sys_err_num] = {
    {E_SUCCESS,       "Success"                     },
    {E_FAILED,        "Failed",                     },
    {E_INVAL,         "Invalid arguments"           },
    {E_TIMEDOUT,      "Timedout"                    },
    {E_BUSY,          "Busy"                        },

//    {E_MAX,           "Max error code"              }
};

const char *
__sys_err::what (uint32_t _code)
{
    assert(_code == __sys_err::__err_info_entry[_code]._code);
    if (_code >= E_MAX)
        throw exception("out of range");
    return __sys_err::__err_info_entry[_code]._str;
}

__NAMESPACE_END

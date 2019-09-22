#include "define.h"
#include "error.h"

__NAMESPACE_BEGIN

const __err_info __err_code_entry[__ERR_CODE_NUM] = {
    {E_SUCCESS,       "Success"                     },
    {E_FAILED,        "Failed",                     },
    {E_INVAL,         "Invalid arguments"           },
    {E_TIMEDOUT,      "Timedout"                    },
    {E_BUSY,          "Busy"                        },

//    {E_MAX,           "Max error code"              }
};

__NAMESPACE_END

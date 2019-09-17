#ifndef _ERROR_H_
#define _ERROR_H_

#include "common.h"

/*
 * Error code
 */
enum __ERR_CODE {
    __ERR_CODE_SUCCESS = 0x1000, // success
    __ERR_CODE_MAX
};
#define __ERR_CODE_BEGIN    __ERR_CODE_SUCCESS
#define __ERR_CODE_END      (__ERR_CODE_MAX - 1)
#define __ERR_CODE_NUM      (__ERR_CODE_MAX)
#define USER_ERR_CODE_BEGIN (__ERR_CODE_MAX)

#define __make_err_code(e) (-(e))

struct __err_info {
    __ERR_CODE  code;
    const char *str;
};

extern const __err_info __err_code_entry[__ERR_CODE_NUM] = {
    {__ERR_CODE_SUCCESS, "Success"},
};

__NAMESPACE_BEGIN

__NAMESPACE_END

#endif /* _ERROR_H_ */
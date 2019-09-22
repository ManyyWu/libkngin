#ifndef _ERROR_H_
#define _ERROR_H_

#include "common.h"

__NAMESPACE_BEGIN

/*
 * Error code
 */
enum __ERROR_CODE {
    E_SUCCESS        = 0x00000000, // success
    E_FAILED         = 0x00001001, // failed, undefined error
    E_INVAL,        // 0x00001002, // invalid arguments
    E_NOMEM,        // 0x00001003, no memory

    // test
    E_TIMEDOUT,     // 0x00001101, time out
    E_BUSY,         // 0x00001102, busy, try again
    E_QUEUE_FULL,   // 0x00001103, the queue is full
    E_MAX
};
#define __ERR_CODE_BEGIN    E_INVAL
#define __ERR_CODE_END      (E_MAX - 1)
#define __ERR_CODE_NUM      (E_MAX - __ERR_CODE_BEGIN)
#define USER_ERR_CODE_BEGIN (E_MAX + 1)

typedef uint32_t error_t;

#define __make_err_code(e) (-(e))

struct __err_info {
    __ERROR_CODE  _code;
    const char *  _str;
};

extern const __err_info __err_code_entry[__ERR_CODE_NUM];

__NAMESPACE_END

#endif /* _ERROR_H_ */
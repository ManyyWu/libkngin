#ifndef _ERROR_H_
#define _ERROR_H_

#include "define.h"

__NAMESPACE_BEGIN

/*
 * Error code
 */
enum __ERROR_CODE {
    E_SUCCESS               = 0x00000000, // success
    
    // application error code                                  [0x00000001, 0x000000FF]
    E_SERVER_INIT_FAIL      = 0x00000001, // failed
    E_SERVER_NOMEM          = 0x00000002, // no memory 

    // common                                                  [0x00001001, 0x0000FFFF]
    E_FAILED                = 0x00001001, // failed, undefined error
    E_INVAL                 = 0x00001002, // invalid arguments
    E_NOMEM                 = 0x00001003, // no memory

    E_TIMEDOUT              = 0x00001101, // time out
    E_BUSY                  = 0x00001102, // busy, try again
    E_QUEUE_FULL            = 0x00001103, // the queue is full

    // for user                                                [0x00010000, 0xFFFFFFFF]

    E_MAX
};
#define __ERR_CODE_BEGIN    E_INVAL
#define __ERR_CODE_END      (E_MAX - 1)
#define __ERR_CODE_NUM      (E_MAX - __ERR_CODE_BEGIN)
#define USER_ERR_CODE_BEGIN (E_MAX + 1)

#define __EXP   noexcept(false)
#define __NOEXP noexcept(true)

class __sys_err {
    enum {};
    const char *what () const {}
};

#define __make_err_code(e) (-(e))

struct __err_info {
    __ERROR_CODE  _code;
    const char *  _str;
};

#define sys_err int // test

extern const __err_info __err_code_entry[__ERR_CODE_NUM];

__NAMESPACE_END

#endif /* _ERROR_H_ */
#ifndef _ERROR_H_
#define _ERROR_H_

#include <logfile.h>
#include <functional>
#include "define.h"
#include "copyable.h"

__NAMESPACE_BEGIN

enum __ERROR_CODE {
    E_SUCCESS               = 0x00000000, // success
    E_SERVER_INIT_FAIL      = 0x00000001, // failed
    E_SERVER_NOMEM          = 0x00000002, // no memory 

    // common                                                  [0x00000100, 0x00000FFF]
    E_FAILED                = 0x00000100, // failed, undefined error
    E_INVAL                 = 0x00000101, // invalid arguments
    E_NOMEM                 = 0x00000102, // no memory
    E_TIMEDOUT              = 0x00000103, // time out
    E_BUSY                  = 0x00000104, // busy, try again
    E_QUEUE_FULL            = 0x00000105, // the queue is full
    E_MAX
    // for user                                                [0x00010000, 0xFFFFFFFF]
};

typedef uint32_t sys_err;

class __sys_err {
public:
    struct __err_info {
        sys_err      _code;
        const char *  _str;
    };

public:
    static const char *
    what (sys_err _code);

public:
    static const uint32_t user_err_code_begin = E_MAX + 1;
    static const uint32_t __sys_err_begin     = E_MAX;
    static const uint32_t __sys_err_end       = E_MAX - 1;
    static const uint32_t __sys_err_num       = E_MAX;

private:
    static const __err_info __err_info_entry[__sys_err_begin];
};

#define err2str(_code) __sys_err::what((_code))

__NAMESPACE_END

#endif /* _ERROR_H_ */
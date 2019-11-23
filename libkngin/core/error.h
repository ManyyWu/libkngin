#ifndef _ERROR_H_
#define _ERROR_H_

#include <logfile.h>
#include <functional>
#include <cstring>
#include <cerrno>
#include "define.h"
#include "timestamp.h"

KNGIN_NAMESPACE_K_BEGIN
/*
enum __ERROR_CODE {
    E_SUCCESS               = 0x00000000, // success

    // application error
    E_SERVER_INIT_FAIL      = 0x00000001, // serverinit failed

    // serious error                                           [0x00000100, 0x00000FFF]
    E_MAX
    // for user                                                [0x00010000, 0xFFFFFFFF]
};

typedef uint32_t sys_err;

#pragma pack(push, 1)
template <typename __T1, typename __T2>
struct __ret_val {
public:
    __ret_val (__T1 _code, __T2 _val) : code(_code), val(_val) {}

public:
    __T1 code;
    __T2 val;
};

template <typename __T>
struct __ret_void {
public:
    __ret_void (__T _code) : code(_code) {}

public:
    __T code;
};
#pragma pack(pop)
typedef struct __ret_void<sys_err>           ret_void;
typedef struct __ret_val<sys_err, bool>      ret_bool;
typedef struct __ret_val<sys_err, uint8_t>   ret_uint8;
typedef struct __ret_val<sys_err, int8_t>    ret_int8;
typedef struct __ret_val<sys_err, uint16_t>  ret_uint16;
typedef struct __ret_val<sys_err, int16_t>   ret_int16;
typedef struct __ret_val<sys_err, uint32_t>  ret_uint32;
typedef struct __ret_val<sys_err, int32_t>   ret_int32;
typedef struct __ret_val<sys_err, uint64_t>  ret_uint64;
typedef struct __ret_val<sys_err, int64_t>   ret_int64;
typedef struct __ret_val<sys_err, size_t>    ret_size;
typedef struct __ret_val<sys_err, ssize_t>   ret_ssize;
typedef struct __ret_val<sys_err, timestamp> ret_timestamp;
typedef struct __ret_val<sys_err, uintptr_t> ret_uintptr_t;
typedef struct __ret_val<sys_err, intptr_t>  ret_intptr_t;
typedef struct __ret_void<bool>              retf_void;
typedef struct __ret_val<bool, bool>         retf_bool;
typedef struct __ret_val<bool, uint8_t>      retf_uint8;
typedef struct __ret_val<bool, int8_t>       retf_int8;
typedef struct __ret_val<bool, uint16_t>     retf_uint16;
typedef struct __ret_val<bool, int16_t>      retf_int16;
typedef struct __ret_val<bool, uint32_t>     retf_uint32;
typedef struct __ret_val<bool, int32_t>      retf_int32;
typedef struct __ret_val<bool, uint64_t>     retf_uint64;
typedef struct __ret_val<bool, int64_t>      retf_int64;
typedef struct __ret_val<bool, size_t>       retf_size;
typedef struct __ret_val<bool, ssize_t>      retf_ssize;
typedef struct __ret_val<bool, timestamp>    retf_timestamp;
typedef struct __ret_val<bool, uintptr_t>    retf_uintptr_t;
typedef struct __ret_val<bool, intptr_t>     retf_intptr_t;

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
*/
KNGIN_NAMESPACE_K_END

#endif /* _ERROR_H_ */
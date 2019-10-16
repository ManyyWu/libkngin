#ifndef _ERROR_H_
#define _ERROR_H_

#include "define.h"
#include "copyable.h"
#include <logfile.h>

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
/*
class __sys_err {
//    const char *what () const {}
};
*/

typedef uint32_t sys_err;

#define __make_err_code(e) (-(e))

typedef uint32_t err_code;

struct __err_info {
    err_code      _code;
    const char *  _str;
};

extern const __err_info __err_code_entry[__ERR_CODE_NUM];

#pragma pack(push, 1)
template <class __T>
class ret_val {
private:
    ret_val  () {}

public:
    ret_val  (__T _val) : m_code(E_SUCCESS), m_value(_val), m_handled(true) {}

    ret_val  (const ret_val &_val) : m_code(_val.m_code), m_value(_val.m_value) {}

    ~ret_val ()
    { if (!m_handled && m_code) log_warning("unsolved error - %s:%d", what(), m_code); }

public:
    __T
    value             () const         { return m_value; }

    ret_val &
    operator =        (__T _val)       { m_value = _val; return *this; }

public:
    err_code
    code              ()               { m_handled = true; return m_code;}

    void
    clear             ()               { m_handled = true; }

    void
    set_code          (err_code _code) { m_code = _code; m_handled = false; }

    explicit
    operator bool     ()               { m_handled = true; return m_code; }

    explicit
    operator err_code ()               { m_handled = true; return m_code; }

    const char *
    what              ()               { m_handled = true; return __err_code_entry[m_code]._str; }

protected:
    err_code         m_code;

    __T              m_value;

    std::atomic_bool m_handled;
};
#pragma pack(pop)

typedef class ret_val<bool>      ret_bool;
typedef class ret_val<uint8_t>   ret_uint8;
typedef class ret_val<int8_t>    ret_int8;
typedef class ret_val<uint16_t>  ret_uint16;
typedef class ret_val<int16_t>   ret_int16;
typedef class ret_val<uint32_t>  ret_uint32;
typedef class ret_val<int32_t>   ret_int32;
typedef class ret_val<uint64_t>  ret_uint64;
typedef class ret_val<int64_t>   ret_int64;
typedef class ret_val<size_t>    ret_size;
typedef class ret_val<ssize_t>   ret_ssize;
typedef class ret_val<timestamp> ret_timestamp;
typedef class ret_val<uintptr_t> ret_uintptr_t;
typedef class ret_val<intptr_t>  ret_intptr_t;

__NAMESPACE_END

#endif /* _ERROR_H_ */
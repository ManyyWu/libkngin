#ifndef _BITS_H_
#define _BITS_H_

#include "define.h"
#include "exception.h"
#include "common.h"

#define __type_bits(__t)            (sizeof(__t) * 8)
#define __bit_value(__t, __p)       ((__t)1 << (__t)(__p))
#define __is_bit_set(__t, __v, __p) ((bool)(((__t)1 << (__t)(__p)) & (__t)(__v)))
#define __set_bit(__t, __v, __p)    ((bool)(((__t)1 << (__t)(__p)) | (__t)(__v)))
#define __clear_bit(__t, __v, __p)  ((bool)(~((__t)1 << (__t)(__p)) & (__t)(__v)))

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/bits.cpp"

__NAMESPACE_BEGIN

template<typename __T>
static inline __T
set_bits (__T _val)
{
    return 0;
}

template<typename __T>
static inline bool
is_bits_set (__T _val)
{
    return true;
}

template<typename __T, typename __T1, typename ...Args>
inline __T
set_bits (__T _val, __T1 _first, Args... _args)
{
    if_not (_first >= 0 && (size_t)_first < __type_bits(__T))
        throw exception("invalid arguments");
    return __bit_value(__T, _first) | set_bits(_val, _args...);
}

template<typename __T, typename __T1, typename ...Args>
inline __T
clear_bits (__T _val, __T1 _first, Args... _args)
{
    if_not (_first >= 0 && (size_t)_first < __type_bits(__T))
        throw exception("invalid arguments");
    return __clear_bit(__T, _val, _first) & set_bits(_val, _args...);
}

template<typename __T, typename ...Args>
inline __T
bits_value (Args... _args)
{
    return set_bits(0, _args...);
}

template<typename __T, typename __T1, typename ...Args>
inline bool
is_bits_set (__T _val, __T1 _first, Args... _args)
{
    if_not (_first >= 0 && (size_t)_first < __type_bits(__T))
        throw exception("invalid arguments");
    return __is_bit_set(__T, _val, _first) && is_bits_set(_val, _args...);
}

__NAMESPACE_END

#endif /* _BITS_H_ */
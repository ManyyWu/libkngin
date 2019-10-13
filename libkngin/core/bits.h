#ifndef _BITS_H_
#define _BITS_H_

#include "define.h"
#include "exception.h"
#include "common.h"

#define TYPE_BITS(__t)        (sizeof(__t) * 8)
#define BITS_VALUE(__t, __p)  ((__t)1 << (__t)(__p))
#define IS_SET(__t, __v, __p) ((bool)(((__t)1 << (__t)(__p)) & (__t)(__v)))

__NAMESPACE_BEGIN

template<typename __T>
static inline __T
set_bits (__T _val)
{
    return 0;
}

template<typename __T>
static inline bool
is_set (__T _val)
{
    return true;
}

template<typename __T, typename __T1, typename ...Args>
inline __T
set_bits (__T _val, __T1 _first, Args... _args)
{
    if_not (_first >= 0 && _first < TYPE_BITS(__T))
        throw exception("invalid arguments");
    return BITS_VALUE(__T, _first) | set_bits(_val, _args...);
}

template<typename __T, typename ...Args>
inline __T
bits_value (Args... _args)
{
    return set_bits(0, _args...);
}

template<typename __T, typename __T1, typename ...Args>
inline bool
is_set (__T _val, __T1 _first, Args... _args)
{
    if_not (_first >= 0 && _first < TYPE_BITS(__T))
        throw exception("invalid arguments");
    return IS_SET(__T, _val, _first) && is_set(_val, _args...);
}

__NAMESPACE_END

#endif /* _BITS_H_ */
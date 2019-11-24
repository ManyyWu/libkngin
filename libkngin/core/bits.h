#ifndef _BITS_H_
#define _BITS_H_

#include "define.h"
#include "exception.h"
#include "common.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/bits.cpp"

#define type_bits(type)              (sizeof(type) * 8)
#define bit_value(type, pos)         ((type)1 << (type)(pos))
#define is_bit_set(type, value, pos) ((bool)(((type)1 << (type)(pos)) & (type)(value)))
#define set_bit(type, value, pos)    ((bool)(((type)1 << (type)(pos)) | (type)(value)))
#define clear_bit(type, value, pos)  ((bool)(~((type)1 << (type)(pos)) & (type)(value)))

KNGIN_NAMESPACE_K_BEGIN

template<typename Type>
static inline Type
set_bits (Type _val)
{
    return 0;
}

template<typename Type>
static inline bool
is_bits_set (Type _val)
{
    return true;
}

template<typename Type, typename Type1, typename ...Args>
inline Type
set_bits (Type _val, Type1 _first, Args... _args)
{
    if_not (_first >= 0 && (size_t)_first < type_bits(Type))
        throw k::exception("out of range");
    return bit_value(Type, _first) | set_bits(_val, _args...);
}

template<typename Type, typename Type1, typename ...Args>
inline Type
clear_bits (Type _val, Type1 _first, Args... _args)
{
    if_not (_first >= 0 && (size_t)_first < type_bits(Type))
        throw k::exception("out of range");
    return clear_bit(Type, _val, _first) & set_bits(_val, _args...);
}

template<typename Type, typename ...Args>
inline Type
bits_value (Args... _args)
{
    return set_bits(0, _args...);
}

template<typename Type, typename Type1, typename ...Args>
inline bool
is_bits_set (Type _val, Type1 _first, Args... _args)
{
    if_not (_first >= 0 && (size_t)_first < type_bits(Type))
        throw k::exception("out of range");
    return is_bit_set(Type, _val, _first) && is_bits_set(_val, _args...);
}

KNGIN_NAMESPACE_K_END

#endif /* _BITS_H_ */
#ifndef KNGIN_BITS_H
#define KNGIN_BITS_H

#include "core/base/define.h"
#include "core/base/exception.h"
#include "core/base/common.h"

#define KNGIN_FILENAME "libkngin/core/base/bits.cpp"

#define TYPE_BITS(type)              (sizeof(type) * 8)
#define BIT_VALUE(type, pos)         ((type)1 << (type)(pos))
#define IS_BIT_SET(type, value, pos) ((bool)(((type)1 << (type)(pos)) & (type)(value)))
#define SET_BIT(type, value, pos)    ((bool)(((type)1 << (type)(pos)) | (type)(value)))
#define CLEAR_BIT(type, value, pos)  ((bool)(~((type)1 << (type)(pos)) & (type)(value)))

KNGIN_NAMESPACE_K_BEGIN

template<typename Type>
static
inline
Type
set_bits (Type _val)
{
    return 0;
}

template<typename Type>
static
inline
bool
is_bits_set (Type _val)
{
    return true;
}

template<typename Type, typename Type1, typename ...Args>
inline
Type
set_bits (Type _val, Type1 _first, Args... _args)
{
    if (_first < 0 or static_cast<size_t>(_first) >= TYPE_BITS(Type))
        throw k::exception("out of range");
    return BIT_VALUE(Type, _first) | set_bits(_val, _args...);
}

template<typename Type, typename Type1, typename ...Args>
inline
Type
clear_bits (Type _val, Type1 _first, Args... _args)
{
    if (_first < 0 or static_cast<size_t>(_first) >= TYPE_BITS(Type))
        throw k::exception("out of range");
    return CLEAR_BIT(Type, _val, _first) & set_bits(_val, _args...);
}

template<typename Type, typename ...Args>
inline
Type
bits_value (Args... _args)
{
    return set_bits(0, _args...);
}

template<typename Type, typename Type1, typename ...Args>
inline
bool
is_bits_set (Type _val, Type1 _first, Args... _args)
{
    if (_first < 0 or static_cast<size_t>(_first) >= TYPE_BITS(Type))
        throw k::exception("out of range");
    return IS_BIT_SET(Type, _val, _first) and is_bits_set(_val, _args...);
}

KNGIN_NAMESPACE_K_END

#undef KNGIN_FILENAME

#endif /* KNGIN_BITS_H */
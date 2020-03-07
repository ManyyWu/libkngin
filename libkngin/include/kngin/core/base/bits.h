#ifndef KNGIN_BITS_H
#define KNGIN_BITS_H

#include "kngin/core/base/exception.h"

#define TYPE_BITS(type)              static_cast<size_t>(sizeof(type) * 8)
#define BIT_VALUE(type, pos)         (static_cast<type>(1) << (pos))
#define IS_BIT_SET(type, value, pos) ((static_cast<type>(1) << (pos)) & static_cast<type>(value))
#define SET_BIT(type, value, pos)    ((static_cast<type>(1) << (pos)) | static_cast<type>(value)))
#define CLEAR_BIT(type, value, pos)  (~(static_cast<type>(1) << (pos)) & static_cast<type>(value))

KNGIN_NAMESPACE_K_BEGIN

template<typename Type>
static
inline
Type
set_bits (Type val) {
  return val;
}

template<typename Type>
static
inline
Type
clear_bits (Type val) {
  return val;
}

template<typename Type>
static
inline
bool
is_bits_set (Type val) {
  return true;
}

template<typename Type, typename Type1, typename ...Args>
inline
Type
set_bits (Type val, Type1 first, Args... args) {
  if (first < 0 or size_t(first) >= TYPE_BITS(Type))
    throw_exception("out of range");
  return (BIT_VALUE(Type, first) | set_bits(val, args...));
}

template<typename Type, typename Type1, typename ...Args>
inline
Type
clear_bits (Type val, Type1 first, Args... args) {
  if (first < 0 or size_t(first) >= TYPE_BITS(Type))
    throw_exception("out of range");
  return (CLEAR_BIT(Type, val, first) & clear_bits(val, args...));
}

template<typename Type, typename ...Args>
inline
Type
bits_value (Args... args) {
  return set_bits(0, args...);
}

template<typename Type, typename Type1, typename ...Args>
inline
bool
is_bits_set (Type val, Type1 first, Args... args) {
  if (first < 0 or size_t(first) >= TYPE_BITS(Type))
    throw_exception("out of range");
  return IS_BIT_SET(Type, val, first) and is_bits_set(val, args...);
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_BITS */
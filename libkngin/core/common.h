#ifndef _COMMON_H_
#define _COMMON_H_

#define __STDC_FORMAT_MACROS
#include <new>
#include <memory>
#include <string>
#include <cinttypes>
#include <cassert>
#include "core/exception.h"
#include "core/define.h"
#include "core/logfile.h"

KNGIN_NAMESPACE_K_BEGIN

// for memory
#define safe_release(ptr)       do { delete (ptr); (ptr) = nullptr; } while (false)
#define safe_release_array(ptr) do { delete [] (ptr); (ptr) = nullptr; } while (false)

// for arguments
inline void
arg_check_func(bool _exp, const char *_what = nullptr)
{
    if (!(_exp) ? assert((_exp)), true : false)
        throw k::exception((std::string(_what ? "invalid argument" : "%s - invalid argument")
                           + _what).c_str());
}
#define arg_check(exp)             do {                                         \
                                       arg_check_func(static_cast<bool>((exp)));\
                                   } while (false)
#define arg_check2(exp, what)      do {                                                 \
                                       arg_check_func(static_cast<bool>((exp)), (what));\
                                   } while (false)

// for expression checking
#define if_not(exp)               if (!(exp)                                   \
                                      ? (assert_log(expression (exp) is false),\
                                         assert((exp)),                        \
                                         true                                  \
                                         )                                     \
                                      : false)
#define check(exp)                do { if_not(exp) (void)0; } while (false)
#define ignore_exp(exp)           do {                                                            \
                                      try {                                                       \
                                          {exp;}                                                  \
                                      } catch (const std::exception &_e) {                        \
                                          log_debug("caught an exception be ignored - %s",        \
                                                    _e.what());                                   \
                                          assert(0);                                              \
                                      } catch (...) {                                             \
                                          log_warning("caught an undefined exception be ignored");\
                                          assert(0);                                              \
                                      }                                                           \
                                  } while (false)
#define log_exp_fatal(exp, ...)   do {                                 \
                                      try {                            \
                                          {exp;}                       \
                                      } catch (...) {                  \
                                          log_fatal("%s", __VA_ARGS__);\
                                          throw;                       \
                                      }                                \
                                  } while (false)
#define log_exp_error(exp, ...)   do {                                 \
                                      try {                            \
                                          {exp;}                       \
                                      } catch (...) {                  \
                                          log_error("%s", __VA_ARGS__);\
                                          throw;                       \
                                      }                                \
                                  } while (false)
#define log_exp_warning(exp, ...) do {                                  \
                                      try {                             \
                                          {exp;}                        \
                                      } catch (...) {                   \
                                          log_warning("%", __VA_ARGS__);\
                                          throw;                        \
                                      }                                 \
                                  } while (false)

// nullptr reference
template <typename Type>
Type &
nullptr_ref () KNGIN_NOEXP
{
    return *static_cast<Type *>(nullptr);
}

template <typename Type>
bool
is_nullptr_ref (Type &_ref) KNGIN_NOEXP
{
    return (nullptr == _ref);
}

// for std::shared_ptr
template <typename Type>
inline bool
is_single_ref_ptr (std::shared_ptr<Type> &_ptr)
{
    return (1 == _ptr.use_count());
}

KNGIN_NAMESPACE_K_END

#endif /* _COMMON_H_ */
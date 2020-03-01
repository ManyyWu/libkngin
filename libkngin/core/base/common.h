#ifndef KNGIN_COMMON_H
#define KNGIN_COMMON_H

#define __STDC_FORMAT_MACROS
#include <new>
#include <memory>
#include <string>
#include <cinttypes>
#include <cassert>
#include "core/base/define.h"
#include "core/base/exception.h"
#include "core/base/logfile.h"

KNGIN_NAMESPACE_K_BEGIN

// for memory
#define safe_release(ptr)       do { delete (ptr); (ptr) = nullptr; } while (false)
#define safe_release_array(ptr) do { delete [] (ptr); (ptr) = nullptr; } while (false)

// for arguments
inline
void
arg_check_func(bool exp, const char *what = nullptr)
{
    if (!(exp) ? assert((exp)), true : false)
        throw k::exception(((what ? "invalid argument - " : "invalid argument")
                             + std::string(what ? what : "")).c_str());
}
#define arg_check(exp)             arg_check_func(static_cast<bool>((exp)))
#define arg_check2(exp, what)      arg_check_func(static_cast<bool>((exp)), (what))

// for expression checking
#define if_not(exp)                  if (!(exp)                                   \
                                         ? (assert_log(expression (exp) is false),\
                                            assert((exp)),                        \
                                            true                                  \
                                            )                                     \
                                         : false)
#define check(exp)                   do { if_not(exp) static_cast<void>(0); } while (false)
#define ignore_excp(exp)             do {                                                            \
                                         try {                                                       \
                                             {exp;}                                                  \
                                         } catch (const std::exception &e) {                        \
                                             log_debug("caught an exception be ignored - %s",        \
                                                       e.what());                                   \
                                         } catch (...) {                                             \
                                             log_warning("caught an undefined exception be ignored");\
                                         }                                                           \
                                     } while (false)
#define cond_excp(exp, what)        do {                              \
                                         if ((exp)) {                  \
                                             log_fatal("%s", (what)); \
                                             throw exception(what);   \
                                         }                             \
                                     } while (false)
#define cond_sys_err(exp, what)     do {                              \
                                         if ((exp)) {                  \
                                             log_fatal("%s", (what)); \
                                             throw system_error(what);\
                                         }                             \
                                     } while (false)
#define log_excp_fatal(exp, what)   do {                              \
                                         try {                         \
                                             {exp;}                    \
                                         } catch (...) {               \
                                             log_fatal("%s", what);   \
                                             throw;                    \
                                         }                             \
                                     } while (false)
#define log_excp_error(exp, what)   do {                              \
                                         try {                         \
                                             {exp;}                    \
                                         } catch (...) {               \
                                             log_error("%s", what);   \
                                             throw;                    \
                                         }                             \
                                     } while (false)
#define log_excp_warning(exp, what) do {                              \
                                         try {                         \
                                             {exp;}                    \
                                         } catch (...) {               \
                                             log_warning("%", what);  \
                                             throw;                    \
                                         }                             \
                                     } while (false)

// nullptr reference
template <typename Type>
Type &
nullptr_ref () noexcept
{
    return *static_cast<Type *>(nullptr);
}

template <typename Type>
bool
is_nullptr_ref (Type &ref) noexcept
{
    return (!ref);
}

// for std::shared_ptr
template <typename Type>
inline
bool
is_single_ref_ptr (std::shared_ptr<Type> &ptr)
{
    return (1 == ptr.use_count());
}

// version
inline
uint32_t
kngin_version ()
{
    return KNGIN_VERSION;
}

inline
const char *
kngin_version_str ()
{
    return KNGIN_VERSION_STR;
}

// others
#define nouse_var(var) (void)(var)
#define shield_var(var) void *var = nullptr; nouse_var(var);

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_COMMON_H */
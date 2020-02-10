#ifndef KNGIN_DEFINE_H
#define KNGIN_DEFINE_H

#define KNGIN_VERSION     010
#define KNGIN_VERSION_STR "0.1.0"

#include "core/base/config.h"

// types
#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

// for exception
//#define KNGIN_EXCP noexcept(false)
#define KNGIN_NOEXCP noexcept(true)

// namespace
#define KNGIN_NAMESPACE_K_BEGIN   namespace k {
#define KNGIN_NAMESPACE_K_END     };
#define KNGIN_NAMESPACE_TCP_BEGIN namespace tcp {
#define KNGIN_NAMESPACE_TCP_END   };
#define KNGIN_NAMESPACE_UDP_BEGIN namespace udp {
#define KNGIN_NAMESPACE_UDP_END   };

// for reference argument
#define LREF
#define RREF
#define CLREF
#define CRREF
#define PTR
#define CPTR

// for timer
#ifdef _WIN32
#define KNGIN_USE_TIMERFD OFF
#endif

#endif /* KNGIN_DEFINE_H */
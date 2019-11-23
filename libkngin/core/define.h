#ifndef _DEFINE_H_
#define _DEFINE_H_

#define KNGIN_VERSION     010
#define KNGIN_VERSION_STR "0.1.0"

#if (__cplusplus < 201103L)
#error c++ version is too low, please build the project using c++11 standard.
#endif

#include <cstdint>
#include <climits>

// types
#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

// compile options
#define ON                     true
#define OFF                    false
#define KNGIN_ENABLE_LOG_MUTEX ON

// namespace
#define KNGIN_NAMESPACE_K_BEGIN   namespace k {
#define KNGIN_NAMESPACE_K_END     };
#define KNGIN_NAMESPACE_TCP_BEGIN namespace tcp {
#define KNGIN_NAMESPACE_TCP_END   };
#define KNGIN_NAMESPACE_UDP_BEGIN namespace udp {
#define KNGIN_NAMESPACE_UDP_END   };

// params
#define __lref
#define __rref
#define __clref
#define __crref
#define __ptr
#define __cptr

// for MSVC
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

// for queues
#define QUEUE_MAX (size_t)(SIZE_MAX)
#define QUEUE_MIN (size_t)1

// for log
#define KNGIN_LOG_RELATIVE_PATH OFF

#endif /* _DEFINE_H_ */
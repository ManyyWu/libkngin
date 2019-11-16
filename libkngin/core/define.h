#ifndef _DEFINE_H_
#define _DEFINE_H_

#if __cplusplus < 201103L
//#error c++ version is too low, please build the project using c++11 standard.
#endif

#include <cstdint>
#include <climits>

// types
#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

// compile options
#define __LOG_MUTEX
#define __DUMP_ERROR_MSG

// namespace
#define __NAMESPACE_BEGIN     namespace k {
#define __NAMESPACE_END       };
#define __API_NAMESPACE_BEGIN namespace api {
#define __API_NAMESPACE_END   };

// queue
#define QUEUE_MAX     (size_t)(SIZE_MAX)
#define QUEUE_MIN     (size_t)10

// for MSVC
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

// for log
//#define __LOG_RELATIVE_PATH

#endif /* _DEFINE_H_ */
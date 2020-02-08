#ifndef KNGIN_DEFINE_H
#define KNGIN_DEFINE_H

#define KNGIN_VERSION     010
#define KNGIN_VERSION_STR "0.1.0"

#if (__cplusplus < 201402L)
#error c++ version is too low, please build the project using c++11 standard.
#endif

#include <cstdint>
#include <climits>

#define ON                     true
#define OFF                    false

// types
#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

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

// for MSVC
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

// for log
#define KNGIN_ASYNC_LOGGER         ON
#define KNGIN_ASYNC_LOGGER_TIMEOUT 3000

// for exception
//#define KNGIN_EXCP   noexcept(false)
#define KNGIN_NOEXCP noexcept(true)

// for queues
#define KNGIN_QUEUE_MAX size_t(SIZE_MAX)
#define KNGIN_QUEUE_MIN size_t(1)

// for log
#define KNGIN_LOG_RELATIVE_PATH OFF

// for session
#define KNGIN_ONE_LOOP_PER_SESSION  ON
#define KNGIN_SESSION_TEMP_CALLBACK ON
#define KNGIN_SESSION_ET_MODE       ON

// for epoller
#define KNGIN_RESERVED_EPOLLELR_EVENT 64
#define KNGIN_EPOLLER_TIMEOUT         3000

#endif /* KNGIN_DEFINE_H */
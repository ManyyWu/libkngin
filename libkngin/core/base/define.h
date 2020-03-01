#ifndef KNGIN_DEFINE_H
#define KNGIN_DEFINE_H

// platform checking
#if !defined(_WIN32) && !defined(TARGET_OS_MAC) && !defined(__linux__) && !defined(__unix__)
#error unsupported platform
#endif

// compiler checking
#if (__cplusplus < 201703L)
#error c++ version is too low, please build the project using c++17 standard.
#endif

// version
#define KNGIN_VERSION     010
#define KNGIN_VERSION_STR "0.1.0"

// includes
#if defined(_WIN32)
#include <Winsock2.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
#endif
#include <ciso646>
#include "core/base/config.h"

// types
#ifdef _WIN32
#if defined(_WIN64)
typedef __int64           ssize_t;
#else
typedef _W64 unsigned int ssize_t;
#endif
#endif

// namespace
#define KNGIN_NAMESPACE_K_BEGIN   namespace k {
#define KNGIN_NAMESPACE_K_END     };
#define KNGIN_NAMESPACE_TCP_BEGIN namespace tcp {
#define KNGIN_NAMESPACE_TCP_END   };
#define KNGIN_NAMESPACE_UDP_BEGIN namespace udp {
#define KNGIN_NAMESPACE_UDP_END   };

// log
#define FILENAME __FILE__
#if (ON == KNGIN_LOG_RELATIVE_PATH)
#undef FILENAME
#define FILENAME KNGIN_FILENAME
#endif
#ifdef NDEBUG
#define FUNCTION __FUNCTION__
#else
#define FUNCTION ""
#endif

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

// flags
#if defined(_WIN32)
#define KNGIN_FLAG_HAVE_HANDLE
#endif

#if !defined(_WIN32)
#define KNGIN_FLAG_HAVE_FILEFD 
#define KNGIN_FLAG_HAVE_EPOLLER
#define KNGIN_FLAG_HAVE_EPOLLER_EVENT
#endif

#if !defined(_WIN32)
#define KNGIN_FLAG_HAVE_EVENTFD
#endif

#if defined(TARGET_OS_MAC)
#define KNGIN_FLAG_HAVE_EVENTFD_WINE
#endif

#endif /* KNGIN_DEFINE_H */
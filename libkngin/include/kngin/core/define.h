#ifndef KNGIN_DEFINE_H
#define KNGIN_DEFINE_H

// includes
#include "kngin/config.h"
#include <ciso646>
#include <cinttypes>
#include <climits>
#include <cstddef>

// version
#define KNGIN_VERSION 0.10lf
#define KNGIN_VERSION_STR "0.1.0"

// for MSVC
#if defined(KNGIN_SYSTEM_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <Winsock2.h>
#  include <Windows.h>
#  pragma comment(lib,"ws2_32.lib")
#  pragma warning(disable: 4996)
#endif /* defined(KNGIN_SYSTEM_WIN32) */

// types
#if defined(KNGIN_SYSTEM_WIN32)
#  if defined(KNGIN_SYSTEM_WIN64)
    typedef __int64           ssize_t;
#  else
    typedef _W64 unsigned int ssize_t;
#  endif /* defined(KNGIN_SYSTEM_WIN64) */
#endif /* defined(KNGIN_SYSTEM_WIN32) */

// namespace
#define KNGIN_NAMESPACE_K_BEGIN      namespace k {
#define KNGIN_NAMESPACE_K_END        };
#define KNGIN_NAMESPACE_DETAIL_BEGIN namespace detail {
#define KNGIN_NAMESPACE_DETAIL_END   };
#define KNGIN_NAMESPACE_IMPL_BEGIN   namespace impl {
#define KNGIN_NAMESPACE_IMPL_END     };
#define KNGIN_NAMESPACE_TCP_BEGIN    namespace tcp {
#define KNGIN_NAMESPACE_TCP_END      };
#define KNGIN_NAMESPACE_UDP_BEGIN    namespace udp {
#define KNGIN_NAMESPACE_UDP_END      };

// log
#if defined(KNGIN_LOG_RELATIVE_PATH)
#  undef FILENAME
#  define FILENAME (__FILE__ + g_path_prefix_size)
#endif /* (ON == KNGIN_LOG_RELATIVE_PATH) */

#endif /* KNGIN_DEFINE_H */

